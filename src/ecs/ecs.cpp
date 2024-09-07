#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/dev/store.hpp>

namespace ntt::ecs
{
    using namespace log;
    using namespace dev::store;

    using component_id_t = entity_id_t;

    struct SystemInfo
    {
        String name;
        SystemFunc systemFunc;
        List<std::type_index> componentTypes;
        List<entity_id_t> entities;

        SystemInfo(String name,
                   SystemFunc systemFunc,
                   List<std::type_index> componentTypes)
            : name(name), systemFunc(systemFunc),
              componentTypes(componentTypes)
        {
        }
    };

    struct EntityInfo
    {
        String name;
        Dictionary<std::type_index, Ref<ComponentBase>> components;

        EntityInfo(String name, Dictionary<std::type_index, Ref<ComponentBase>> components)
            : name(name), components(components)
        {
        }
    };

    using ComponentStore = Store<component_id_t, ComponentBase, entity_id_t>;
    using system_id_t = u32;

    namespace
    {
        b8 s_isInitialized = FALSE;

        Scope<Store<system_id_t, SystemInfo, String>> s_systemsStore;
        Scope<Store<entity_id_t, EntityInfo, String>> s_entityStore;
    } // namespace

    void ECSInit()
    {
        if (s_isInitialized)
        {
            return;
        }

        s_entityStore = CreateScope<Store<entity_id_t, EntityInfo, String>>(
            0,
            1000,
            [](Ref<EntityInfo> entity) -> String
            { return entity->name; });

        s_systemsStore = CreateScope<Store<system_id_t, SystemInfo, String>>(
            0,
            1000,
            [](Ref<SystemInfo> system) -> String
            { return system->name; });

        s_isInitialized = TRUE;
    }

    void ECSRegister(String name, SystemFunc systemFunc,
                     List<std::type_index> componentTypes)
    {
        if (!s_isInitialized)
        {
            return;
        }

        s_systemsStore->Add(CREATE_REF(
            SystemInfo,
            name,
            systemFunc,
            componentTypes));

        for (auto i = 0; i < componentTypes.size(); i++)
        {
            std::type_index type = componentTypes[i];
        }
    }

    List<entity_id_t> ECSGetEntitiesWithSystem(String name)
    {
        if (!s_isInitialized)
        {
            return {};
        }

        auto system = s_systemsStore->GetByUnique(name);

        if (system == nullptr)
        {
            return {};
        }

        return system->entities;
    }

    b8 _IsEntityInSystem(system_id_t system_id, entity_id_t entity_id)
    {
        auto system = s_systemsStore->Get(system_id);
        auto entities = system->entities;
        auto systemTypes = system->componentTypes;

        auto components = s_entityStore->Get(entity_id)->components;

        b8 isValid = TRUE;
        for (auto j = 0; j < systemTypes.size(); j++)
        {
            auto type = systemTypes[j];

            if (!components.Contains(type))
            {
                return FALSE;
            }
        }

        return isValid;
    }

    entity_id_t ECSCreateEntity(String name, Dictionary<std::type_index, Ref<ComponentBase>> components)
    {
        if (!s_isInitialized)
        {
            return 0;
        }

        auto entityId = s_entityStore->Add(CREATE_REF(EntityInfo, name, components));
        NTT_ENGINE_TRACE("Creating entity: {}", entityId);

        components.ForEach([&entityId](const std::type_index &, const Ref<ComponentBase> &component)
                           { component->entity_id = entityId; });

        auto availableSystems = s_systemsStore->GetAvailableIds();

        for (auto systemId : availableSystems)
        {
            if (_IsEntityInSystem(systemId, entityId))
            {
                auto system = s_systemsStore->Get(systemId);
                system->entities.push_back(entityId);
            }
        }

        return entityId;
    }

    Ref<ComponentBase> ECSGetComponent(entity_id_t id, std::type_index type)
    {
        if (!s_isInitialized)
        {
            return nullptr;
        }

        if (!s_entityStore->Contains(id))
        {
            return nullptr;
        }

        auto entityInfo = s_entityStore->Get(id);

        if (!entityInfo->components.Contains(type))
        {
            return nullptr;
        }

        return entityInfo->components[type];
    }

    void ECSSetComponentActive(entity_id_t id, std::type_index type, b8 active)
    {
        if (!s_isInitialized)
        {
            return;
        }

        if (!s_entityStore->Contains(id))
        {
            // NTT_ENGINE_WARN("The entity with ID {} is not existed", id);
            return;
        }

        auto entityInfo = s_entityStore->Get(id);

        if (!entityInfo->components.Contains(type))
        {
            // NTT_ENGINE_WARN("The component with type {} is not existed in the entity", type.name());
            return;
        }

        auto component = entityInfo->components[type];
        component->active = active;

        auto availableSystems = s_systemsStore->GetAvailableIds();
        for (auto systemId : availableSystems)
        {
            auto system = s_systemsStore->Get(systemId);

            if (!system->componentTypes.Contains(type))
            {
                continue;
            }

            if (active)
            {
                if (system->entities.Contains(id))
                {
                    continue;
                }

                if (_IsEntityInSystem(systemId, id))
                {
                    system->entities.push_back(id);
                }
            }
            else
            {
                if (system->entities.Contains(id))
                {
                    system->entities.RemoveItem(id);
                }
            }
        }
    }

    void ECSDeleteEntity(entity_id_t id)
    {
        if (!s_isInitialized)
        {
            return;
        }

        if (!s_entityStore->Contains(id))
        {
            // NTT_ENGINE_WARN("The entity with ID {} is not existed", id);
            return;
        }

        auto entityInfo = s_entityStore->Get(id);
        NTT_ENGINE_TRACE("Deleting entity: {}", id);

        auto keyComponents = entityInfo->components.Keys();

        auto availabeSystems = s_systemsStore->GetAvailableIds();

        for (auto systemId : availabeSystems)
        {
            auto system = s_systemsStore->Get(systemId);
            system->entities.RemoveItem(id);
        }

        for (auto component : entityInfo->components)
        {
            component.second.reset();
        }

        s_entityStore->Release(id);
    }

    void ECSUpdate(f32 delta)
    {
        if (!s_isInitialized)
        {
            return;
        }

        auto availableIds = s_entityStore->GetAvailableIds();
        auto availableSystems = s_systemsStore->GetAvailableIds();

        for (auto systemId : availableSystems)
        {
            auto system = s_systemsStore->Get(systemId);
            auto entities = system->entities;

            for (auto entityId : system->entities)
            {
                if (!s_entityStore->Contains(entityId))
                {
                    continue;
                }

                try
                {
                    system->systemFunc(delta, entityId);
                }
                catch (const std::exception &e)
                {
                    NTT_ENGINE_ERROR("Error in system: {} - System: {}", e.what(), system->name);
                }
            }
        }
    }

    void ECSShutdown()
    {
        if (!s_isInitialized)
        {
            return;
        }

        auto availableIds = s_entityStore->GetAvailableIds();

        for (auto entityId : availableIds)
        {
            ECSDeleteEntity(entityId);
        }

        s_entityStore.reset();
        s_systemsStore.reset();

        s_isInitialized = FALSE;
    }
} // namespace ntt::ecs
