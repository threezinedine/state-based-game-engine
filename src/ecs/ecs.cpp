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
        Scope<System> system;
        List<std::type_index> componentTypes;

        SystemInfo(String name,
                   Scope<System> system,
                   List<std::type_index> componentTypes)
            : name(name), system(std::move(system)),
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

    void ECSRegister(String name, CreateSystemFunc createSystemFunc, List<std::type_index> componentTypes)
    {
        if (!s_isInitialized)
        {
            return;
        }

        s_systemsStore->Add(CREATE_REF(
            SystemInfo,
            name,
            createSystemFunc(),
            componentTypes));

        for (auto i = 0; i < componentTypes.Length(); i++)
        {
            std::type_index type = componentTypes[i];
        }
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

        return entityInfo->components.Get(type);
    }

    void ECSDeleteEntity(entity_id_t id)
    {
        if (!s_isInitialized)
        {
            return;
        }

        if (!s_entityStore->Contains(id))
        {
            NTT_ENGINE_WARN("The entity with ID {} is not existed", id);
            return;
        }

        auto entityInfo = s_entityStore->Get(id);
        NTT_ENGINE_TRACE("Deleting entity: {}", id);

        auto keyComponents = entityInfo->components.Keys();

        for (auto i = 0; i < keyComponents.Length(); i++)
        {
            entityInfo->components.Get(keyComponents[i]).reset();
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

        for (auto i = 0; i < availableSystems.Length(); i++)
        {
            auto systemInfo = s_systemsStore->Get(availableSystems[i]);
            auto systemTypes = systemInfo->componentTypes;
            List<entity_id_t> entities = {};

            for (auto j = 0; j < availableIds.Length(); j++)
            {
                b8 isValid = FALSE;
                auto entity_id = availableIds[j];
                auto entityInfo = s_entityStore->Get(entity_id);
                for (auto k = 0; k < systemTypes.Length(); k++)
                {
                    auto type = systemTypes[k];
                    if (!entityInfo->components.Contains(type))
                    {
                        isValid = FALSE;
                        break;
                    }
                    isValid = TRUE;
                }

                if (isValid)
                {
                    try
                    {
                        systemInfo->system->Update(delta, entity_id);
                    }
                    catch (std::exception &e)
                    {
                        NTT_ENGINE_ERROR(
                            "Error when updating system: {} in system: {}",
                            e.what(), systemInfo->name);
                    }
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

        for (auto i = 0; i < availableIds.Length(); i++)
        {
            ECSDeleteEntity(availableIds[i]);
        }

        s_entityStore.reset();
        s_systemsStore.reset();

        s_isInitialized = FALSE;
    }
} // namespace ntt::ecs
