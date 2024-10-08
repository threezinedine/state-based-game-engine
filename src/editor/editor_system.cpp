#include <NTTEngine/editor/editor_system.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/renderer/renderer.hpp>
#include <NTTEngine/application/script_system/script_store.hpp>
#include <NTTEngine/application/script_system/native_script.hpp>
#include <NTTEngine/application/script_system/script_component.hpp>
#include <NTTEngine/renderer/Parent.hpp>

#include "controllers/MoveXController.hpp"
#include "controllers/MoveAroundController.hpp"
#include "controllers/MoveYController.hpp"

#define CENTER_SIZE 20 ///< The size of the center rect

namespace ntt
{
    using namespace log;
    using namespace renderer;
    using namespace input;
    using namespace script;

    class EditorSystem::Impl
    {
    public:
        List<entity_id_t> selectedEntities;
        Dictionary<entity_id_t, List<entity_id_t>> drawnEntities;
        List<entity_id_t> allDrawnEntities;

        resource_id_t moveXControllerScriptId;
        resource_id_t moveYControllerScriptId;
        resource_id_t moveAroundControllerScriptId;

        u16 currentLayer = EDITOR_LAYER;

        void OnLayerChanged(event_code_t code, void *sender, const EventContext &context)
        {
            u16 newLayer = context.u16_data[0];
            if (newLayer == EDITOR_LAYER)
            {
                return;
            }

            currentLayer = newLayer;
            Clear();
        }

        void Clear()
        {
            for (auto selected : selectedEntities)
            {
                for (auto btnId : drawnEntities[selected])
                {
                    ECSDeleteEntity(btnId);
                    allDrawnEntities.RemoveItem(btnId);
                }

                drawnEntities[selected].clear();
            }

            selectedEntities.clear();
        }

        void ChooseNewEntity(entity_id_t entityId)
        {
            selectedEntities.push_back(entityId);

            if (!drawnEntities.Contains(entityId))
            {
                drawnEntities[entityId] = List<entity_id_t>();
            }

            auto geo = ECS_GET_COMPONENT(entityId, Geometry);

            if (geo == nullptr)
            {
                return;
            }

            ECSBeginLayer(EDITOR_LAYER);
            auto center = ECSCreateEntity(
                "Center Rect",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x,
                        geo->y,
                        CENTER_SIZE,
                        CENTER_SIZE,
                        geo->rotation,
                        PRIORITY_0,
                        NTT_RED),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        moveAroundControllerScriptId,
                        INVALID_OBJECT_ID,
                        &entityId),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, 0, 0),
                });

            position_t axisWidth = 100;

            auto xAxis = ECSCreateEntity(
                "X Axis",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x + axisWidth / 2 + CENTER_SIZE / 2, geo->y, axisWidth, 3, 0.0f,
                        PRIORITY_0, NTT_GREEN),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, axisWidth / 2 + CENTER_SIZE / 2, 0),
                });

            auto xPoint = ECSCreateEntity(
                "X point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x + axisWidth + CENTER_SIZE / 2, geo->y, CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_0, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, axisWidth + CENTER_SIZE / 2, 0),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        moveXControllerScriptId,
                        INVALID_OBJECT_ID,
                        &entityId),
                });

            auto yAxis = ECSCreateEntity(
                "Y Axis",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x, geo->y - axisWidth / 2 - CENTER_SIZE / 2, 3, axisWidth, 0.0f,
                        PRIORITY_0, NTT_GREEN),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, 0, -axisWidth / 2 - CENTER_SIZE / 2),
                });

            auto yPoint = ECSCreateEntity(
                "Y point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x, geo->y - axisWidth - CENTER_SIZE / 2, CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_0, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, 0, -axisWidth - CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        moveYControllerScriptId,
                        INVALID_OBJECT_ID,
                        &entityId),
                });

            drawnEntities[entityId].push_back(center);
            drawnEntities[entityId].push_back(xAxis);
            drawnEntities[entityId].push_back(xPoint);
            drawnEntities[entityId].push_back(yAxis);
            drawnEntities[entityId].push_back(yPoint);

            allDrawnEntities.push_back(center);
            allDrawnEntities.push_back(xAxis);
            allDrawnEntities.push_back(yAxis);
            allDrawnEntities.push_back(xPoint);
            allDrawnEntities.push_back(yPoint);

            ECSBeginLayer(GAME_LAYER);
        }

        void OnEditorChooseEntity(event_code_t code, void *sender, const EventContext &context)
        {
            auto entityId = context.u32_data[0];
            Clear();
            ChooseNewEntity(entityId);
        }

        void OnEditorAppendEntity(event_code_t code, void *sender, const EventContext &context)
        {
            auto entityId = context.u32_data[0];
            ChooseNewEntity(entityId);
        }

        void OnEditorSelectedMoveRequest(event_code_t code, void *sender, const EventContext &context)
        {
            f32 x = context.f32_data[0];
            f32 y = context.f32_data[1];

            entity_id_t entityId = reinterpret_cast<Script *>(sender)->GetEntity();

            if (selectedEntities.size() == 0)
            {
                return;
            }

            if (selectedEntities.Contains(entityId))
            {
                return;
            }

            TriggerEvent(NTT_EDITOR_SELECTED_MOVE, &selectedEntities, context);
            return;
        }
    };

    EditorSystem::EditorSystem()
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
    }

    EditorSystem::~EditorSystem()
    {
        PROFILE_FUNCTION();
    }

    void EditorSystem::InitSystem()
    {
        PROFILE_FUNCTION();

        m_impl->selectedEntities.clear();
        m_impl->drawnEntities.clear();

        m_impl->moveXControllerScriptId = ScriptStoreLoad(
            "move-x-controller",
            [](void *data) -> Ref<void>
            { return std::reinterpret_pointer_cast<void>(CreateRef<MoveXController>(data)); },
            [](Ref<void> script)
            {
                std::reinterpret_pointer_cast<MoveXController>(script).reset();
            },
            []() -> std::type_index
            {
                return typeid(MoveXController);
            });

        m_impl->moveAroundControllerScriptId = ScriptStoreLoad(
            "move-around-controller",
            [](void *data) -> Ref<void>
            { return std::reinterpret_pointer_cast<void>(CreateRef<MoveAroundController>(data)); },
            [](Ref<void> script)
            {
                std::reinterpret_pointer_cast<MoveAroundController>(script).reset();
            },
            []() -> std::type_index
            {
                return typeid(Script);
            });

        m_impl->moveYControllerScriptId = ScriptStoreLoad(
            "move-y-controller",
            [](void *data) -> Ref<void>
            { return std::reinterpret_pointer_cast<void>(CreateRef<MoveYController>(data)); },
            [](Ref<void> script)
            {
                std::reinterpret_pointer_cast<MoveYController>(script).reset();
            },
            []() -> std::type_index
            {
                return typeid(Script);
            });

        RegisterEvent(NTT_LAYER_CHANGED,
                      std::bind(&Impl::OnLayerChanged,
                                m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_CHOOSE_ENTITY,
                      std::bind(&Impl::OnEditorChooseEntity,
                                m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_APPEND_ENTITY,
                      std::bind(&Impl::OnEditorAppendEntity,
                                m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_SELECTED_MOVE_REQUEST,
                      std::bind(&Impl::OnEditorSelectedMoveRequest,
                                m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(
            NTT_SCENE_CHANGED,
            [&](event_code_t code, void *sender, const EventContext &context)
            { m_impl->Clear(); });
    }

    void EditorSystem::InitEntity(entity_id_t entityId)
    {
        PROFILE_FUNCTION();
    }

    void EditorSystem::Update(f32 delta, entity_id_t entityId)
    {
        PROFILE_FUNCTION();

        auto hoveredIds = GetHoveredTexture();

        if (hoveredIds.size() == 0)
        {
            return;
        }

        if (hoveredIds[hoveredIds.size() - 1] != entityId)
        {
            return;
        }

        if (m_impl->allDrawnEntities.Contains(entityId))
        {
            return;
        }

        if (!CheckState(NTT_BUTTON_LEFT, NTT_PRESS))
        {
            return;
        }

        if (!CheckState(NTT_KEY_LEFT_CONTROL, NTT_DOWN) &&
            !CheckState(NTT_KEY_RIGHT_CONTROL, NTT_DOWN))
        {
            EventContext context;
            context.u32_data[0] = entityId;
            TriggerEvent(NTT_EDITOR_CHOOSE_ENTITY, nullptr, context);
        }
        else
        {
            EventContext context;
            context.u32_data[0] = entityId;
            TriggerEvent(NTT_EDITOR_APPEND_ENTITY, nullptr, context);
        }
    }

    void EditorSystem::ShutdownEntity(entity_id_t entityId)
    {
        PROFILE_FUNCTION();
    }

    void EditorSystem::ShutdownSystem()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt
