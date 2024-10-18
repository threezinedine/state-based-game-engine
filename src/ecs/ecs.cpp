#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/dev/store.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/ecs/data_com.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/core/object.hpp>
#include <cstring>
#include <NTTEngine/core/auto_naming.hpp>

namespace ntt
{
    using component_id_t = entity_id_t;
#define INVALID_UI_LAYER 255

    struct SystemInfo : public Object
    {
        String name;
        Ref<System> system;
        List<std::type_index> componentTypes;
        List<entity_id_t> entities;
        b8 alwayUpdate = FALSE;
        b8 active = TRUE;

        SystemInfo(String name,
                   Ref<System> system,
                   List<std::type_index> componentTypes,
                   b8 alwayUpdate = FALSE)
            : name(name), system(system),
              componentTypes(componentTypes),
              alwayUpdate(alwayUpdate),
              active(TRUE)
        {
        }
    };

    using system_id_t = u32;

    namespace
    {
        Scope<Store<system_id_t, SystemInfo>> s_systemsStore;
        Scope<Store<entity_id_t, EntityInfo>> s_entityStore;

        List<entity_id_t> s_DrawnEntities;
        List<entity_id_t> s_UpdatedEntities;

        Scope<List<entity_id_t>> layers[MAX_LAYERS];
        List<b8> layersVisibility;
        layer_t currentLayer = GAME_LAYER;
        layer_t currentRunningLayer = GAME_LAYER;
        layer_t preLayer = GAME_LAYER;
        u8 uiLayerVisible = INVALID_UI_LAYER;

        List<entity_id_t> s_selectedEntities;

        // void InitEditor(event_code_t code, void *sender, const EventContext &context)
        // {
        //     PROFILE_FUNCTION();

        //     preLayer = currentRunningLayer;
        //     ECSLayerMakeVisible(EDITOR_LAYER);
        // }

        // void EditorRun(event_code_t code, void *sender, const EventContext &context)
        // {
        //     PROFILE_FUNCTION();

        //     ECSLayerMakeVisible(preLayer);
        // }

        void ResetEntitiesState()
        {
            PROFILE_FUNCTION();

            s_DrawnEntities.clear();
            s_UpdatedEntities.clear();

            s_DrawnEntities.insert(
                s_DrawnEntities.end(),
                layers[GAME_LAYER]->begin(),
                layers[GAME_LAYER]->end());

            if (uiLayerVisible != INVALID_UI_LAYER)
            {
                s_DrawnEntities.insert(
                    s_DrawnEntities.end(),
                    layers[uiLayerVisible]->begin(),
                    layers[uiLayerVisible]->end());
            }

            if (currentRunningLayer == EDITOR_LAYER)
            {
                s_DrawnEntities.insert(
                    s_DrawnEntities.end(),
                    layers[EDITOR_LAYER]->begin(),
                    layers[EDITOR_LAYER]->end());
            }

            for (auto entity : *(layers[currentRunningLayer]))
            {
                s_UpdatedEntities.push_back(entity);
            }
        }

        List<entity_id_t> s_deletedEntities;

        void InternalEntityDelete(entity_id_t id)
        {
            PROFILE_FUNCTION();

            if (!s_entityStore->Contains(id))
            {
                // NTT_ENGINE_WARN("The entity with ID {} is not existed", id);
                return;
            }

            auto entityInfo = s_entityStore->Get(id);
            NTT_ENGINE_TRACE("Deleting entity: {}", id);

            auto keyComponents = entityInfo->components.Keys();

            auto availabeSystems = s_systemsStore->GetAvailableIds();

            for (auto entityCom : entityInfo->components.Keys())
            {
                ECSSetComponentActive(id, entityCom, TRUE);
            }

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

            for (auto &layer : layers)
            {
                if (layer == nullptr)
                {
                    continue;
                }
                layer->RemoveItem(id);
            }

            ResetEntitiesState();

            EventContext context;
            // must be changed when the entity_id_t is changed
            context.u32_data[0] = id;
            TriggerEvent(NTT_ENTITY_DESTROYED, nullptr, context);
        }

        b8 IsEntityInSystem(system_id_t system_id, entity_id_t entity_id)
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

    } // namespace

    void ECSInit()
    {
        PROFILE_FUNCTION();

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

        s_DrawnEntities.clear();
        s_UpdatedEntities.clear();
        s_deletedEntities.clear();

        s_selectedEntities.clear();

        // RegisterEvent(NTT_LAYER_CHANGED, std::bind(OnSceneOpened));
        memset(layers, 0, sizeof(layers));

        for (auto i = 0; i < MAX_LAYERS; i++)
        {
            layers[i] = CreateScope<List<entity_id_t>>();
        }

        currentLayer = GAME_LAYER;
        currentRunningLayer = GAME_LAYER;
        uiLayerVisible = INVALID_UI_LAYER;
    }

    void ECSRegister(String name, Ref<System> system,
                     List<std::type_index> componentTypes,
                     b8 alwayUpdate)
    {
        PROFILE_FUNCTION();

        auto systemId = s_systemsStore->Add(CreateRef<SystemInfo>(
            // GetName(name),
            name,
            system,
            componentTypes,
            alwayUpdate));

        for (auto i = 0; i < componentTypes.size(); i++)
        {
            std::type_index type = componentTypes[i];
        }

        system->InitSystem();
    }

    void ECSChangeSystemState(String name, b8 active)
    {
        PROFILE_FUNCTION();

        auto system = s_systemsStore->GetByField<String>(
            name,
            [](Ref<SystemInfo> system) -> String
            { return system->name; });

        if (system.size() != 1)
        {
            NTT_ENGINE_WARN("The system with name {} is not found", name);
            return;
        }

        system[0]->active = active;
    }

    void ECSBeginLayer(layer_t layer)
    {
        PROFILE_FUNCTION();

        if (layer >= MAX_LAYERS)
        {
            return;
        }

        currentLayer = layer;

        EventContext context;
        context.u16_data[0] = currentLayer;
        TriggerEvent(NTT_DEFINED_LAYER_CHANGED, nullptr, context);
    }

    void ECS_ClearLayer(layer_t layer)
    {
        PROFILE_FUNCTION();

        if (layer >= MAX_LAYERS)
        {
            return;
        }

        List<entity_id_t> entities = *layers[layer];

        for (auto entity : entities)
        {
            InternalEntityDelete(entity);
        }
    }

    void ECSLayerMakeVisible(layer_t layer)
    {
        PROFILE_FUNCTION();

        if (layer >= MAX_LAYERS)
        {
            return;
        }

        if (GAME_LAYER < layer && layer < EDITOR_LAYER)
        {
            uiLayerVisible = layer;
        }

        if (layer == GAME_LAYER)
        {
            uiLayerVisible = INVALID_UI_LAYER;
        }

        currentRunningLayer = layer;

        ResetEntitiesState();

        EventContext context;
        context.u16_data[0] = currentRunningLayer;
        TriggerEvent(NTT_LAYER_CHANGED, nullptr, context);
    }

    List<entity_id_t> ECSGetEntitiesWithSystem(String name)
    {
        PROFILE_FUNCTION();

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

    entity_id_t ECSCreateEntity(
        const String &name,
        Dictionary<std::type_index, Ref<ComponentBase>> components)
    {
        PROFILE_FUNCTION();

        components[typeid(DataComponent)] = CreateRef<DataComponent>();

        auto entityInfo = CreateRef<EntityInfo>();
        entityInfo->components = components;
        auto entityId = s_entityStore->Add(entityInfo);
        s_entityStore->Get(entityId)->name = name;

        components.ForEach(
            [&entityId](const std::type_index &,
                        const Ref<ComponentBase> &component)
            { component->entity_id = entityId; });

        auto availableSystems = s_systemsStore->GetAvailableIds();

        for (auto systemId : availableSystems)
        {
            if (IsEntityInSystem(systemId, entityId))
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

        if (currentLayer > MAX_LAYERS)
        {
            return INVALID_ENTITY_ID;
        }

        if (layers[currentLayer]->Contains(entityId))
        {
            return INVALID_ENTITY_ID;
        }

        layers[currentLayer]->push_back(entityId);

        auto geo = ECS_GET_COMPONENT(entityId, Geometry);

        if (geo != nullptr)
        {
            geo->priority += (currentLayer * LAYER_PRIORITY_RANGE);
        }

        ResetEntitiesState();

        NTT_ENGINE_TRACE("Entit {} has id {}", name, entityId);
        EventContext context;
        // must be changed when the entity_id_t is changed
        context.u32_data[0] = entityId;
        TriggerEvent(NTT_ENTITY_CREATED, nullptr, context);

        // OnSceneOpened();

        return entityId;
    }

    Ref<EntityInfo> ECSGetEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();

        return s_entityStore->Get(id);
    }

    entity_id_t ECSGetEntityByName(const String &name)
    {
        PROFILE_FUNCTION();

        List<entity_id_t> ids = s_entityStore->GetIdsByField<String>(
            name,
            [](Ref<EntityInfo> entity) -> String
            { return entity->name; });

        if (ids.size() != 1)
        {
            return INVALID_ENTITY_ID;
        }

        return ids[0];
    }

    Ref<ComponentBase> ECSGetEntityComponent(entity_id_t id, std::type_index type)
    {
        PROFILE_FUNCTION();

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

        if (!s_entityStore->Contains(id))
        {
            NTT_ENGINE_TRACE("The entity with ID {} is not existed", id);
            return;
        }

        auto entityInfo = s_entityStore->Get(id);

        if (!entityInfo->components.Contains(type))
        {
            NTT_ENGINE_TRACE("The component with type {} is not existed in the entity",
                             type.name());
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

                if (IsEntityInSystem(systemId, id))
                {
                    system->entities.push_back(id);
                    system->entities.Sorted();
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
        // Delay delete the entity until the end of the frame (ECSUpdate)

        // if (s_deletedEntities.Contains(id))
        // {
        //     return;
        // }

        // s_deletedEntities.push_back(id);
        if (id == INVALID_ENTITY_ID)
        {
            return;
        }

        InternalEntityDelete(id);
    }

    void ECSUpdate(f32 delta)
    {
        PROFILE_FUNCTION();

        auto availableIds = s_entityStore->GetAvailableIds();
        auto availableSystems = s_systemsStore->GetAvailableIds();

        for (auto systemId : availableSystems)
        {
            auto system = s_systemsStore->Get(systemId);

            if (!system->active)
            {
                continue;
            }

            auto entities = system->entities;

            for (auto entityId : system->entities)
            {
                if (system->alwayUpdate)
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

        // for (auto entityId : s_deletedEntities)
        // {
        //     InternalEntityDelete(entityId);
        // }

        // s_deletedEntities.clear();
    }

    void ECSRemoveAllEntities()
    {
        PROFILE_FUNCTION();

        auto availableIds = s_entityStore->GetAvailableIds();

        for (auto entityId : availableIds)
        {
            InternalEntityDelete(entityId);
        }
    }

    void ECSShutdown()
    {
        PROFILE_FUNCTION();

        auto availableIds = s_entityStore->GetAvailableIds();

        for (auto entityId : availableIds)
        {
            // ECSDeleteEntity(entityId);
            InternalEntityDelete(entityId);
        }

        auto availableSystems = s_systemsStore->GetAvailableIds();
        for (auto systemId : availableSystems)
        {
            auto system = s_systemsStore->Get(systemId);
            system->system->ShutdownSystem();
        }

        for (auto &layer : layers)
        {
            layer.reset();
            ASSERT_M(layer == nullptr, "The layer is not reset properly");
        }

        s_entityStore.reset();
        s_systemsStore.reset();
    }
} // namespace ntt
