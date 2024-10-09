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

#include "controllers/TransformScript.hpp"
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

        resource_id_t transformScriptId;

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

            TransformScriptData centerData;
            centerData.entity = entityId;
            centerData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->x += x;
                geo->y += y;
            };

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
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &centerData),
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

            TransformScriptData xPointData;
            xPointData.entity = entityId;
            xPointData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->x += x;
            };

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
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &xPointData),
                });

            TransformScriptData xPointNegData;
            xPointNegData.entity = entityId;
            xPointNegData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->x += x;
            };

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
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &xPointNegData),
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

            TransformScriptData yPointData;
            yPointData.entity = entityId;
            yPointData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->y += y;
            };

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
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &yPointData),
                });

            TransformScriptData yPointNegData;
            yPointNegData.entity = entityId;
            yPointNegData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->y += y;
            };

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
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &yPointNegData),
                });

            TransformScriptData leftTopData;
            leftTopData.entity = entityId;
            leftTopData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->posX = -geo->width / 2 - CENTER_SIZE / 2;
                parent->posY = -geo->height / 2 - CENTER_SIZE / 2;
            };

            leftTopData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->width -= x * 2;
                geo->height -= y * 2;
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
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &leftTopData),
                });

            TransformScriptData rightTopData;
            rightTopData.entity = entityId;
            rightTopData.onAddEntReset =
                [](
                    entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->posX = geo->width / 2 + CENTER_SIZE / 2;
                parent->posY = -geo->height / 2 - CENTER_SIZE / 2;
            };

            rightTopData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->width += x * 2;
                geo->height -= y * 2;
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
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &rightTopData),
                });

            TransformScriptData rightBottomData;
            rightBottomData.entity = entityId;
            rightBottomData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->posX = geo->width / 2 + CENTER_SIZE / 2;
                parent->posY = geo->height / 2 + CENTER_SIZE / 2;
            };

            rightBottomData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->width += x * 2;
                geo->height += y * 2;
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
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &rightBottomData),
                });

            TransformScriptData leftBottomData;
            leftBottomData.entity = entityId;
            leftBottomData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->posX = -geo->width / 2 - CENTER_SIZE / 2;
                parent->posY = geo->height / 2 + CENTER_SIZE / 2;
            };

            leftBottomData.onResizeMain =
                [](f32 x, f32 y, Ref<Geometry> geo)
            {
                geo->width -= x * 2;
                geo->height += y * 2;
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
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &leftBottomData),
                });

            TransformScriptData leftPointData;
            leftPointData.entity = entityId;
            leftPointData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->width -= x * 2;
            };

            leftPointData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->posX = -geo->width / 2 - CENTER_SIZE / 2;
            };

            auto leftPoint = ECSCreateEntity(
                "left-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x - geo->width / 2 - CENTER_SIZE / 2,
                        geo->y, CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_1, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        -geo->width / 2 - CENTER_SIZE / 2,
                        0),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &leftPointData),
                });

            TransformScriptData rightPointData;
            rightPointData.entity = entityId;
            rightPointData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->width += x * 2;
            };

            rightPointData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->posX = geo->width / 2 + CENTER_SIZE / 2;
            };

            auto rightPoint = ECSCreateEntity(
                "right-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x + geo->width / 2 + CENTER_SIZE / 2,
                        geo->y, CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_1, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        geo->width / 2 + CENTER_SIZE / 2,
                        0),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &rightPointData),
                });

            TransformScriptData topPointData;
            topPointData.entity = entityId;
            topPointData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->height -= y * 2;
            };

            topPointData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->posY = -geo->height / 2 - CENTER_SIZE / 2;
            };

            auto topPoint = ECSCreateEntity(
                "top-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x, geo->y - geo->height / 2 - CENTER_SIZE / 2,
                        CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_1, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        0,
                        -geo->height / 2 - CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &topPointData),
                });

            TransformScriptData bottomPointData;
            bottomPointData.entity = entityId;
            bottomPointData.onResizeMain =
                [](
                    f32 x, f32 y,
                    Ref<Geometry> geo)
            {
                geo->height += y * 2;
            };

            bottomPointData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->posY = geo->height / 2 + CENTER_SIZE / 2;
            };

            auto bottomPoint = ECSCreateEntity(
                "bottom-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->x, geo->y + geo->height / 2 + CENTER_SIZE / 2,
                        CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_1, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        0,
                        geo->height / 2 + CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &bottomPointData),
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
            drawnEntities[entityId].push_back(leftPoint);
            drawnEntities[entityId].push_back(rightPoint);
            drawnEntities[entityId].push_back(topPoint);
            drawnEntities[entityId].push_back(bottomPoint);

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
            resizeEntities.push_back(leftPoint);
            resizeEntities.push_back(rightPoint);
            resizeEntities.push_back(topPoint);
            resizeEntities.push_back(bottomPoint);

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
            allDrawnEntities.push_back(leftPoint);
            allDrawnEntities.push_back(rightPoint);
            allDrawnEntities.push_back(topPoint);
            allDrawnEntities.push_back(bottomPoint);

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

            for (auto entityId : selectedEntities)
            {
                auto geo = ECS_GET_COMPONENT(entityId, Geometry);
                if (geo == nullptr)
                {
                    continue;
                }

                OnTransformFunc onResizeMain =
                    static_cast<TransformScriptData *>(sender)
                        ->onResizeMain;

                if (onResizeMain != nullptr)
                {
                    onResizeMain(x, y, geo);
                }
            }

            TriggerEvent(NTT_EDITOR_TRANSFORM_CHANGED, nullptr, context);
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

        m_impl->transformScriptId = ScriptStoreLoad(
            "transform-script",
            [](void *data) -> Ref<void>
            { return std::reinterpret_pointer_cast<void>(CreateRef<TransformScript>(data)); },
            [](Ref<void> script)
            {
                std::reinterpret_pointer_cast<TransformScript>(script).reset();
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
            NTT_EDITOR_TRANSFORM_CHANGED_REQUEST,
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
