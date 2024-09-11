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
        System system;
        List<std::type_index> componentTypes;
        List<entity_id_t> entities;

        SystemInfo(String name,
                   System system,
                   List<std::type_index> componentTypes)
            : name(name), system(system),
              componentTypes(componentTypes)
        {
        }
    };

    struct EntityInfo
    {
        Dictionary<std::type_index, Ref<ComponentBase>> components;

        EntityInfo(Dictionary<std::type_index, Ref<ComponentBase>> components)
            : components(components)
        {
        }
    };

    using system_id_t = u32;

    namespace
    {
        b8 s_isInitialized = FALSE;

        Scope<Store<system_id_t, SystemInfo>> s_systemsStore;
        Scope<Store<entity_id_t, EntityInfo>> s_entityStore;
    } // namespace

    void ECSInit()
    {
        if (s_isInitialized)
        {
            return;
        }

        s_entityStore = CreateScope<Store<entity_id_t, EntityInfo>>(
            0,
            1000,
            [](Ref<EntityInfo> a, Ref<EntityInfo> b) -> b8
            { return FALSE; });

        s_systemsStore = CreateScope<Store<system_id_t, SystemInfo>>(
            0,
            1000,
            [](Ref<SystemInfo> a, Ref<SystemInfo> b) -> b8
            { return a->name == b->name; });

        s_isInitialized = TRUE;
    }

    void ECSRegister(String name, System system,
                     List<std::type_index> componentTypes)
    {
        if (!s_isInitialized)
        {
            return;
        }

        s_systemsStore->Add(CREATE_REF(
            SystemInfo,
            name,
            system,
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

        auto system = s_systemsStore->GetByField<String>(
            name,
            [](Ref<SystemInfo> system) -> String
            { return system->name; });

        if (system.size() != 1)
        {
            return {};
        }

        return system[0]->entities;
    }

    static b8 _IsEntityInSystem(system_id_t system_id, entity_id_t entity_id)
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

    entity_id_t ECSCreateEntity(Dictionary<std::type_index, Ref<ComponentBase>> components)
    {
        if (!s_isInitialized)
        {
            return 0;
        }

        auto entityId = s_entityStore->Add(CREATE_REF(EntityInfo, components));

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
                    system->system.update(delta, entityId);
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
