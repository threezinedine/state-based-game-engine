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
#include "controllers/ResizeController.hpp"
#include "editor_tool.hpp"

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
        List<entity_id_t> moveEntities;
        List<entity_id_t> resizeEntities;

        resource_id_t moveXControllerScriptId;
        resource_id_t moveYControllerScriptId;
        resource_id_t moveAroundControllerScriptId;
        resource_id_t resizeControllerScriptId;

        u16 currentLayer = EDITOR_LAYER;
        ToolType currentTool = MOVE;

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
                    moveEntities.RemoveItem(btnId);
                    resizeEntities.RemoveItem(btnId);
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

            auto xAxisNeg = ECSCreateEntity(
                "X Axis Neg",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x - axisWidth / 2 - CENTER_SIZE / 2, geo->y, axisWidth, 3, 0.0f,
                        PRIORITY_0, NTT_GREEN),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, -axisWidth / 2 - CENTER_SIZE / 2, 0),
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
                        entityId, axisWidth + CENTER_SIZE, 0),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        moveXControllerScriptId,
                        INVALID_OBJECT_ID,
                        &entityId),
                });

            auto xPointNeg = ECSCreateEntity(
                "X point Neg",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x - axisWidth - CENTER_SIZE / 2, geo->y, CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_0, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, -axisWidth - CENTER_SIZE, 0),
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

            auto yAxisNeg = ECSCreateEntity(
                "Y Axis Neg",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x, geo->y + axisWidth / 2 + CENTER_SIZE / 2, 3, axisWidth, 0.0f,
                        PRIORITY_0, NTT_GREEN),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, 0, axisWidth / 2 + CENTER_SIZE / 2),
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
                        entityId, 0, -axisWidth - CENTER_SIZE),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        moveYControllerScriptId,
                        INVALID_OBJECT_ID,
                        &entityId),
                });

            auto yPointNeg = ECSCreateEntity(
                "Y point Neg",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x, geo->y + axisWidth + CENTER_SIZE / 2, CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_0, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, 0, axisWidth + CENTER_SIZE),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        moveYControllerScriptId,
                        INVALID_OBJECT_ID,
                        &entityId),
                });

            ResizeControllerData leftTopData;
            leftTopData.entity = entityId;
            leftTopData.onResize =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo,
                    Ref<Parent> topLeft)
            {
                topLeft->posX = -geo->width / 2 - CENTER_SIZE / 2;
                topLeft->posY = -geo->height / 2 - CENTER_SIZE / 2;
            };

            leftTopData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->x += x / 2;
                geo->y += y / 2;
                geo->width -= x;
                geo->height -= y;
            };

            auto leftTopPoint = ECSCreateEntity(
                "left-top-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x - geo->width / 2 - CENTER_SIZE / 2,
                        geo->y - geo->height / 2 - CENTER_SIZE / 2,
                        CENTER_SIZE, CENTER_SIZE,
                        0.0f, PRIORITY_1, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent, entityId,
                        -geo->width / 2 - CENTER_SIZE / 2,
                        -geo->height / 2 - CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        resizeControllerScriptId,
                        INVALID_OBJECT_ID,
                        &leftTopData),
                });

            ResizeControllerData rightTopData;
            rightTopData.entity = entityId;
            rightTopData.onResize =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo,
                    Ref<Parent> topRight)
            {
                topRight->posX = geo->width / 2 + CENTER_SIZE / 2;
                topRight->posY = -geo->height / 2 - CENTER_SIZE / 2;
            };

            rightTopData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->x += x / 2;
                geo->y += y / 2;
                geo->width += x;
                geo->height -= y;
            };

            auto rightTopPoint = ECSCreateEntity(
                "right-top-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x + geo->width / 2 + CENTER_SIZE / 2,
                        geo->y - geo->height / 2 - CENTER_SIZE / 2,
                        CENTER_SIZE,
                        CENTER_SIZE, 0.0f,
                        PRIORITY_1,
                        NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        geo->width / 2 + CENTER_SIZE / 2,
                        -geo->height / 2 - CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        resizeControllerScriptId,
                        INVALID_OBJECT_ID,
                        &rightTopData),
                });

            ResizeControllerData rightBottomData;
            rightBottomData.entity = entityId;
            rightBottomData.onResize =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo,
                    Ref<Parent> bottomRight)
            {
                bottomRight->posX = geo->width / 2 + CENTER_SIZE / 2;
                bottomRight->posY = geo->height / 2 + CENTER_SIZE / 2;
            };

            rightBottomData.onResizeMain = [](
                                               f32 x, f32 y,
                                               Ref<Geometry> geo)
            {
                geo->x += x / 2;
                geo->y += y / 2;
                geo->width += x;
                geo->height += y;
            };

            auto rightBottomPoint = ECSCreateEntity(
                "right-bottom-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x + geo->width / 2 + CENTER_SIZE / 2,
                        geo->y + geo->height / 2 + CENTER_SIZE / 2,
                        CENTER_SIZE,
                        CENTER_SIZE, 0.0f,
                        PRIORITY_1,
                        NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        geo->width / 2 + CENTER_SIZE / 2,
                        geo->height / 2 + CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        resizeControllerScriptId,
                        INVALID_OBJECT_ID,
                        &rightBottomData),
                });

            ResizeControllerData leftBottomData;
            leftBottomData.entity = entityId;
            leftBottomData.onResize =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo,
                    Ref<Parent> bottomLeft)
            {
                bottomLeft->posX = -geo->width / 2 - CENTER_SIZE / 2;
                bottomLeft->posY = geo->height / 2 + CENTER_SIZE / 2;
            };

            leftBottomData.onResizeMain =
                [](f32 x, f32 y, Ref<Geometry> geo)
            {
                geo->x += x / 2;
                geo->y += y / 2;
                geo->width -= x;
                geo->height += y;
            };

            auto leftBottomPoint = ECSCreateEntity(
                "left-bottom-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x - geo->width / 2 - CENTER_SIZE / 2,
                        geo->y + geo->height / 2 + CENTER_SIZE / 2,
                        CENTER_SIZE,
                        CENTER_SIZE, 0.0f,
                        PRIORITY_1,
                        NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        -geo->width / 2 - CENTER_SIZE / 2,
                        geo->height / 2 + CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        resizeControllerScriptId,
                        INVALID_OBJECT_ID,
                        &leftBottomData),
                });

            drawnEntities[entityId].push_back(center);
            drawnEntities[entityId].push_back(xAxis);
            drawnEntities[entityId].push_back(xPoint);
            drawnEntities[entityId].push_back(yAxis);
            drawnEntities[entityId].push_back(yPoint);
            drawnEntities[entityId].push_back(leftTopPoint);
            drawnEntities[entityId].push_back(rightTopPoint);
            drawnEntities[entityId].push_back(leftBottomPoint);
            drawnEntities[entityId].push_back(rightBottomPoint);
            drawnEntities[entityId].push_back(xAxisNeg);
            drawnEntities[entityId].push_back(xPointNeg);
            drawnEntities[entityId].push_back(yAxisNeg);
            drawnEntities[entityId].push_back(yPointNeg);

            moveEntities.push_back(center);
            moveEntities.push_back(xPoint);
            moveEntities.push_back(yPoint);
            moveEntities.push_back(xAxis);
            moveEntities.push_back(yAxis);
            moveEntities.push_back(xAxisNeg);
            moveEntities.push_back(xPointNeg);
            moveEntities.push_back(yAxisNeg);
            moveEntities.push_back(yPointNeg);

            resizeEntities.push_back(leftTopPoint);
            resizeEntities.push_back(rightTopPoint);
            resizeEntities.push_back(leftBottomPoint);
            resizeEntities.push_back(rightBottomPoint);

            allDrawnEntities.push_back(center);
            allDrawnEntities.push_back(xAxis);
            allDrawnEntities.push_back(yAxis);
            allDrawnEntities.push_back(xPoint);
            allDrawnEntities.push_back(yPoint);
            allDrawnEntities.push_back(leftTopPoint);
            allDrawnEntities.push_back(rightTopPoint);
            allDrawnEntities.push_back(leftBottomPoint);
            allDrawnEntities.push_back(rightBottomPoint);
            allDrawnEntities.push_back(xAxisNeg);
            allDrawnEntities.push_back(xPointNeg);
            allDrawnEntities.push_back(yAxisNeg);
            allDrawnEntities.push_back(yPointNeg);

            ChangeMoveState(currentTool == MOVE);
            ChangeResizeState(currentTool == SCALE);

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

        void OnEditorSelectedRequest(event_code_t code, void *sender, const EventContext &context)
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

            switch (code)
            {
            case NTT_EDITOR_SELECTED_MOVE_REQUEST:
                TriggerEvent(NTT_EDITOR_SELECTED_MOVE, &selectedEntities, context);
                break;
            case NTT_EDITOR_SELECTED_RESIZE_REQUEST:
                TriggerEvent(NTT_EDITOR_SELECTED_RESIZE, &selectedEntities, context);
                break;
            }
            return;
        }

        void ChangeMoveState(b8 active)
        {
            for (auto entityId : moveEntities)
            {
                ECSSetComponentActive(entityId, typeid(NativeScriptComponent), active);
                ECSSetComponentActive(entityId, typeid(Geometry), active);
            }
        }

        void ChangeResizeState(b8 active)
        {
            for (auto entityId : resizeEntities)
            {
                ECSSetComponentActive(entityId, typeid(NativeScriptComponent), active);
                ECSSetComponentActive(entityId, typeid(Geometry), active);
            }
        }

        void OnToolTypeChanged(event_code_t code, void *sender, const EventContext &context)
        {
            auto tool = static_cast<ToolType>(context.u8_data[0]);
            currentTool = tool;

            for (auto entityId : allDrawnEntities)
            {
                ChangeMoveState(currentTool == MOVE);
                ChangeResizeState(currentTool == SCALE);
            }
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

        m_impl->resizeControllerScriptId = ScriptStoreLoad(
            "resize-controller",
            [](void *data) -> Ref<void>
            { return std::reinterpret_pointer_cast<void>(CreateRef<ResizeController>(data)); },
            [](Ref<void> script)
            {
                std::reinterpret_pointer_cast<ResizeController>(script).reset();
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
                      std::bind(&Impl::OnEditorSelectedRequest,
                                m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_SELECTED_RESIZE_REQUEST,
                      std::bind(&Impl::OnEditorSelectedRequest,
                                m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(
            NTT_SCENE_CHANGED,
            [&](event_code_t code, void *sender, const EventContext &context)
            { m_impl->Clear(); });

        RegisterEvent(NTT_EDITOR_TOOL_TYPE_CHANGED,
                      std::bind(&Impl::OnToolTypeChanged,
                                m_impl.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));
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
            if (m_impl->selectedEntities.Contains(entityId))
            {
                return;
            }

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
