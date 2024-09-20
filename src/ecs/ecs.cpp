#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/dev/store.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/ecs/data_com.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/layer_system/layer_system.hpp>
#include <NTTEngine/renderer/RenderSystem.hpp>

namespace ntt::ecs
{
    using namespace log;
    using namespace dev::store;
    using namespace event;
    using namespace renderer;

    using component_id_t = entity_id_t;

    struct SystemInfo
    {
        String name;
        Ref<System> system;
        List<std::type_index> componentTypes;
        List<entity_id_t> entities;

        SystemInfo(String name,
                   Ref<System> system,
                   List<std::type_index> componentTypes)
            : name(name), system(system),
              componentTypes(componentTypes)
        {
        }
    };

    using system_id_t = u32;

    namespace
    {
        b8 s_isInitialized = FALSE;

        Scope<Store<system_id_t, SystemInfo>> s_systemsStore;
        Scope<Store<entity_id_t, EntityInfo>> s_entityStore;

        List<entity_id_t> s_DrawnEntities;
        List<entity_id_t> s_UpdatedEntities;

        void OnSceneOpened()
        {
            PROFILE_FUNCTION();
            s_DrawnEntities = DrawnEntities();
            s_UpdatedEntities = UpdatedEntities();
        }
    } // namespace

    void ECSInit()
    {
        PROFILE_FUNCTION();
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

        s_DrawnEntities.clear();
        s_UpdatedEntities.clear();

        RegisterEvent(NTT_LAYER_CHANGED, std::bind(OnSceneOpened));
    }

    void ECSRegister(String name, Ref<System> system,
                     List<std::type_index> componentTypes)
    {
        PROFILE_FUNCTION();
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

        system->InitSystem();
    }

    List<entity_id_t> ECSGetEntitiesWithSystem(String name)
    {
        PROFILE_FUNCTION();
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
        PROFILE_FUNCTION();
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

    entity_id_t ECSCreateEntity(
        const String &name,
        Dictionary<std::type_index, Ref<ComponentBase>> components)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return 0;
        }

        components[typeid(DataComponent)] = CreateRef<DataComponent>();

        auto entityId = s_entityStore->Add(CREATE_REF(EntityInfo, components));

        components.ForEach(
            [&entityId](const std::type_index &,
                        const Ref<ComponentBase> &component)
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

        // the entity must be added to all the systems which need the components
        //     before the init function is called
        for (auto systemId : availableSystems)
        {
            auto system = s_systemsStore->Get(systemId);
            if (system->entities.Contains(entityId))
            {
                system->system->InitEntity(entityId);
            }
        }

        NTT_ENGINE_TRACE("Entit {} has id {}", name, entityId);
        EventContext context;
        // must be changed when the entity_id_t is changed
        context.u32_data[0] = entityId;
        TriggerEvent(NTT_ENTITY_CREATED, nullptr, context);

        OnSceneOpened();

        return entityId;
    }

    Ref<EntityInfo> ECSGetEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return nullptr;
        }

        return s_entityStore->Get(id);
    }

    Ref<ComponentBase> ECSGetEntityComponent(entity_id_t id, std::type_index type)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return nullptr;
        }

        if (!s_entityStore->Contains(id))
        {
            NTT_ENGINE_TRACE("The entity with ID {} is not existed", id);
            return nullptr;
        }

        auto entityInfo = s_entityStore->Get(id);

        if (!entityInfo->components.Contains(type))
        {
            NTT_ENGINE_TRACE("The component with type {} is not existed in the entity",
                             type.name());
            return nullptr;
        }

        return entityInfo->components[type];
    }

    void ECSSetComponentActive(entity_id_t id, std::type_index type, b8 active)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return;
        }

        if (!s_entityStore->Contains(id))
        {
            NTT_ENGINE_TRACE("The entity with ID {} is not existed", id);
            return;
        }

        auto entityInfo = s_entityStore->Get(id);

        if (!entityInfo->components.Contains(type))
        {
            NTT_ENGINE_TRACE("The component with type {} is not existed in the entity", type.name());
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
        PROFILE_FUNCTION();
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
            if (system->entities.Contains(id))
            {
                system->system->ShutdownEntity(id);
                system->entities.RemoveItem(id);
            }
        }

        for (auto component : entityInfo->components)
        {
            component.second.reset();
        }

        s_entityStore->Release(id);

        EventContext context;
        // must be changed when the entity_id_t is changed
        context.u32_data[0] = id;
        TriggerEvent(NTT_ENTITY_DESTROYED, nullptr, context);

        OnSceneOpened();
    }

    void ECSUpdate(f32 delta)
    {
        PROFILE_FUNCTION();
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

            auto isRenderSystem =
                ::std::dynamic_pointer_cast<RenderSystem>(system->system) ||
                ::std::dynamic_pointer_cast<TextRenderSystem>(system->system);

            for (auto entityId : system->entities)
            {
                if (isRenderSystem)
                {
                    if (!s_DrawnEntities.Contains(entityId))
                    {
                        continue;
                    }
                }
                else
                {
                    if (!s_UpdatedEntities.Contains(entityId))
                    {
                        continue;
                    }
                }

                if (!s_entityStore->Contains(entityId))
                {
                    continue;
                }

                try
                {
                    system->system->Update(delta, entityId);
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
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return;
        }

        auto availableIds = s_entityStore->GetAvailableIds();

        for (auto entityId : availableIds)
        {
            ECSDeleteEntity(entityId);
        }

        auto availableSystems = s_systemsStore->GetAvailableIds();
        for (auto systemId : availableSystems)
        {
            auto system = s_systemsStore->Get(systemId);
            system->system->ShutdownSystem();
        }

        s_entityStore.reset();
        s_systemsStore.reset();

        s_isInitialized = FALSE;
    }
} // namespace ntt::ecs
