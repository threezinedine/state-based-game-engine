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
#include <NTTEngine/platforms/application.hpp>
#include <math.h>
#include <functional>

#include "controllers/TransformScript.hpp"
#include "editor_windows/editor_tool/editor_tool.hpp"

#define CENTER_SIZE 20 ///< The size of the center rect
#define LIMIT_SIZE 20  ///< The limit size of the entity
#define INVALID_NEAREST_POSITION -999999

namespace ntt
{
    class EditorSystem::Impl
    {
    public:
        List<entity_id_t> selectedEntities;
        Dictionary<entity_id_t, List<entity_id_t>> drawnEntities;
        List<entity_id_t> allDrawnEntities;
        List<entity_id_t> moveEntities;
        List<entity_id_t> rotateEntities;
        List<entity_id_t> resizeEntities;

        u16 currentLayer = EDITOR_LAYER;
        ToolType currentTool = MOVE;

        position_t prevX = INVALID_NEAREST_POSITION;
        position_t prevY = INVALID_NEAREST_POSITION;

        void OnLayerChanged(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
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
            PROFILE_FUNCTION();
            for (auto selected : selectedEntities)
            {
                for (auto btnId : drawnEntities[selected])
                {
                    ECSDeleteEntity(btnId);
                    allDrawnEntities.RemoveItem(btnId);
                    moveEntities.RemoveItem(btnId);
                    resizeEntities.RemoveItem(btnId);
                    rotateEntities.RemoveItem(btnId);
                }

                drawnEntities[selected].clear();
            }

            drawnEntities.clear();
            selectedEntities.clear();
        }

        position_t CheckNearXAlign(const Position &pos)
        {
            auto windowSize = GetWindowSize();
            List<position_t> xAlignPoints = {};
            xAlignPoints.push_back(0);
            xAlignPoints.push_back(windowSize.width / 2);
            xAlignPoints.push_back(windowSize.width);

            for (auto align : xAlignPoints)
            {
                if (abs(align - pos.x) < LIMIT_SIZE)
                {
                    return align;
                }
            }

            return INVALID_NEAREST_POSITION;
        }

        position_t CheckNearYAlign(const Position &pos)
        {
            auto windowSize = GetWindowSize();
            List<position_t> yAlignPoints = {};
            yAlignPoints.push_back(0);
            yAlignPoints.push_back(windowSize.height / 2);
            yAlignPoints.push_back(windowSize.height);

            for (auto align : yAlignPoints)
            {
                if (abs(align - pos.y) < LIMIT_SIZE)
                {
                    return align;
                }
            }

            return INVALID_NEAREST_POSITION;
        }

        void ChooseNewEntity(entity_id_t entityId)
        {
            PROFILE_FUNCTION();
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

#define ADD_ALL(entity)                        \
    drawnEntities[entityId].push_back(entity); \
    allDrawnEntities.push_back(entity);        \
    // NTT_ENGINE_DEBUG("Entity {} is added with the id {}", #entity, entity);     \
    // NTT_ENGINE_DEBUG("Drawn entities: {}", drawnEntities[entityId].ToString()); \
    // NTT_ENGINE_DEBUG("All drawn entities: {}", allDrawnEntities.ToString());    \
    // NTT_ENGINE_DEBUG("Move entities: {}", moveEntities.ToString());             \
    // NTT_ENGINE_DEBUG("Resize entities: {}", resizeEntities.ToString());         \
    // NTT_ENGINE_DEBUG("Rotate entities: {}", rotateEntities.ToString());

            std::function<void(const Position &, const Position &)> onEntityChanged =
                [](const Position &start, const Position &end)
            {
                TriggerEvent(NTT_EDITOR_SAVE_SCENE);
            };

            TransformScriptData centerData;
            centerData.entity = entityId;
            centerData.onResizeMain =
                [&](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                if (prevX == INVALID_NEAREST_POSITION)
                {
                    geo->pos.x += delta.x;
                }

                if (prevY == INVALID_NEAREST_POSITION)
                {
                    geo->pos.y += delta.y;
                }

                auto windowSize = GetWindowSize();

                position_t xAlign;

                if (prevX == INVALID_NEAREST_POSITION)
                {
                    xAlign = CheckNearXAlign(geo->pos);
                }
                else
                {
                    xAlign = CheckNearXAlign({prevX, geo->pos.y});
                }

                if (xAlign != INVALID_NEAREST_POSITION)
                {
                    if (prevX == INVALID_NEAREST_POSITION)
                    {
                        prevX = geo->pos.x;
                        geo->pos.x = xAlign;
                    }

                    prevX += delta.x;
                    DrawContext context;
                    context.lineType = 1;
                    context.color = NTT_RED;
                    context.priority = PRIORITY_4;
                    DrawLine({xAlign, 0},
                             {xAlign, windowSize.height},
                             context);
                }
                else
                {
                    if (prevX != INVALID_NEAREST_POSITION)
                    {
                        geo->pos.x = prevX;
                        prevX = INVALID_NEAREST_POSITION;
                    }
                }

                position_t yAlign;

                if (prevY == INVALID_NEAREST_POSITION)
                {
                    yAlign = CheckNearYAlign(geo->pos);
                }
                else
                {
                    yAlign = CheckNearYAlign({geo->pos.x, prevY});
                }

                if (yAlign != INVALID_NEAREST_POSITION)
                {
                    if (prevY == INVALID_NEAREST_POSITION)
                    {
                        prevY = geo->pos.y;
                        geo->pos.y = yAlign;
                    }

                    prevY += delta.y;
                    DrawContext context;
                    context.lineType = 1;
                    context.priority = PRIORITY_4;
                    context.color = NTT_RED;
                    DrawLine({0, yAlign},
                             {windowSize.width, yAlign},
                             context);
                }
                else
                {
                    if (prevY != INVALID_NEAREST_POSITION)
                    {
                        geo->pos.y = prevY;
                        prevY = INVALID_NEAREST_POSITION;
                    }
                }
            };
            centerData.onEntityChanged = onEntityChanged;

            auto center = ECSCreateEntity(
                "Center Rect",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x,
                        geo->pos.y,
                        CENTER_SIZE,
                        CENTER_SIZE,
                        geo->rotation,
                        PRIORITY_0,
                        NTT_RED),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &centerData),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, 0, 0),
                });

            moveEntities.push_back(center);
            ADD_ALL(center);

            position_t axisWidth = 100;

            auto xAxis = ECSCreateEntity(
                "X Axis",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x + axisWidth / 2 + CENTER_SIZE / 2, geo->pos.y,
                        axisWidth, 3, 0.0f,
                        PRIORITY_0, NTT_GREEN),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, axisWidth / 2 + CENTER_SIZE / 2, 0),
                });

            moveEntities.push_back(xAxis);
            ADD_ALL(xAxis);

            auto xAxisNeg = ECSCreateEntity(
                "X Axis Neg",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x - axisWidth / 2 - CENTER_SIZE / 2, geo->pos.y,
                        axisWidth, 3, 0.0f,
                        PRIORITY_0, NTT_GREEN),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, -axisWidth / 2 - CENTER_SIZE / 2, 0),
                });

            moveEntities.push_back(xAxisNeg);
            ADD_ALL(xAxisNeg);

            TransformScriptData xPointData;
            xPointData.entity = entityId;
            xPointData.onResizeMain =
                [&](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                if (prevX == INVALID_NEAREST_POSITION)
                {
                    geo->pos.x += delta.x;
                }

                auto windowSize = GetWindowSize();
                position_t xAlign;

                if (prevX == INVALID_NEAREST_POSITION)
                {
                    xAlign = CheckNearXAlign(geo->pos);
                }
                else
                {
                    xAlign = CheckNearXAlign({prevX, geo->pos.y});
                }

                if (xAlign != INVALID_NEAREST_POSITION)
                {
                    if (prevX == INVALID_NEAREST_POSITION)
                    {
                        prevX = geo->pos.x;
                        geo->pos.x = xAlign;
                    }

                    prevX += delta.x;
                    DrawContext context;
                    context.lineType = 1;
                    context.color = NTT_RED;
                    context.priority = PRIORITY_4;
                    DrawLine({xAlign, 0},
                             {xAlign, windowSize.height},
                             context);
                }
                else
                {
                    if (prevX != INVALID_NEAREST_POSITION)
                    {
                        geo->pos.x = prevX;
                        prevX = INVALID_NEAREST_POSITION;
                    }
                }
            };
            xPointData.onEntityChanged = onEntityChanged;

            auto xPoint = ECSCreateEntity(
                "X point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x + axisWidth + CENTER_SIZE / 2, geo->pos.y,
                        CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_0, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, axisWidth + CENTER_SIZE, 0),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &xPointData),
                });

            moveEntities.push_back(xPoint);
            ADD_ALL(xPoint);

            TransformScriptData xPointNegData;
            xPointNegData.entity = entityId;
            xPointNegData.onResizeMain =
                [&](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                if (prevX == INVALID_NEAREST_POSITION)
                {
                    geo->pos.x += delta.x;
                }

                auto windowSize = GetWindowSize();
                position_t xAlign;

                if (prevX == INVALID_NEAREST_POSITION)
                {
                    xAlign = CheckNearXAlign(geo->pos);
                }
                else
                {
                    xAlign = CheckNearXAlign({prevX, geo->pos.y});
                }

                if (xAlign != INVALID_NEAREST_POSITION)
                {
                    if (prevX == INVALID_NEAREST_POSITION)
                    {
                        prevX = geo->pos.x;
                        geo->pos.x = xAlign;
                    }

                    prevX += delta.x;
                    DrawContext context;
                    context.lineType = 1;
                    context.color = NTT_RED;
                    context.priority = PRIORITY_4;
                    DrawLine({xAlign, 0},
                             {xAlign, windowSize.height},
                             context);
                }
                else
                {
                    if (prevX != INVALID_NEAREST_POSITION)
                    {
                        geo->pos.x = prevX;
                        prevX = INVALID_NEAREST_POSITION;
                    }
                }
            };
            xPointNegData.onEntityChanged = onEntityChanged;

            auto xPointNeg = ECSCreateEntity(
                "X point Neg",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x - axisWidth - CENTER_SIZE / 2, geo->pos.y,
                        CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_0, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, -axisWidth - CENTER_SIZE, 0),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &xPointNegData),
                });

            moveEntities.push_back(xPointNeg);
            ADD_ALL(xPointNeg);

            auto yAxis = ECSCreateEntity(
                "Y Axis",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x, geo->pos.y - axisWidth / 2 - CENTER_SIZE / 2,
                        3, axisWidth, 0.0f,
                        PRIORITY_0, NTT_GREEN),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, 0, -axisWidth / 2 - CENTER_SIZE / 2),
                });

            moveEntities.push_back(yAxis);
            ADD_ALL(yAxis);

            auto yAxisNeg = ECSCreateEntity(
                "Y Axis Neg",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x, geo->pos.y + axisWidth / 2 + CENTER_SIZE / 2,
                        3, axisWidth, 0.0f,
                        PRIORITY_0, NTT_GREEN),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, 0, axisWidth / 2 + CENTER_SIZE / 2),
                });

            moveEntities.push_back(yAxisNeg);
            ADD_ALL(yAxisNeg);

            TransformScriptData yPointData;
            yPointData.entity = entityId;
            yPointData.onResizeMain =
                [&](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                if (prevY == INVALID_NEAREST_POSITION)
                {
                    geo->pos.y += delta.y;
                }

                auto windowSize = GetWindowSize();
                position_t yAlign;

                if (prevY == INVALID_NEAREST_POSITION)
                {
                    yAlign = CheckNearYAlign(geo->pos);
                }
                else
                {
                    yAlign = CheckNearYAlign({geo->pos.x, prevY});
                }

                if (yAlign != INVALID_NEAREST_POSITION)
                {
                    if (prevY == INVALID_NEAREST_POSITION)
                    {
                        prevY = geo->pos.y;
                        geo->pos.y = yAlign;
                    }

                    prevY += delta.y;
                    DrawContext context;
                    context.lineType = 1;
                    context.color = NTT_RED;
                    context.priority = PRIORITY_4;
                    DrawLine({0, yAlign},
                             {windowSize.width, yAlign},
                             context);
                }
                else
                {
                    if (prevY != INVALID_NEAREST_POSITION)
                    {
                        geo->pos.y = prevY;
                        prevY = INVALID_NEAREST_POSITION;
                    }
                }
            };
            yPointData.onEntityChanged = onEntityChanged;

            auto yPoint = ECSCreateEntity(
                "Y point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x, geo->pos.y - axisWidth - CENTER_SIZE / 2,
                        CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_0, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, 0, -axisWidth - CENTER_SIZE),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &yPointData),
                });

            moveEntities.push_back(yPoint);
            ADD_ALL(yPoint);

            TransformScriptData yPointNegData;
            yPointNegData.entity = entityId;
            yPointNegData.onResizeMain =
                [&](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                if (prevY == INVALID_NEAREST_POSITION)
                {
                    geo->pos.y += delta.y;
                }

                auto windowSize = GetWindowSize();
                position_t yAlign;

                if (prevY == INVALID_NEAREST_POSITION)
                {
                    yAlign = CheckNearYAlign(geo->pos);
                }
                else
                {
                    yAlign = CheckNearYAlign({geo->pos.x, prevY});
                }

                if (yAlign != INVALID_NEAREST_POSITION)
                {
                    if (prevY == INVALID_NEAREST_POSITION)
                    {
                        prevY = geo->pos.y;
                        geo->pos.y = yAlign;
                    }

                    prevY += delta.y;
                    DrawContext context;
                    context.lineType = 1;
                    context.color = NTT_RED;
                    context.priority = PRIORITY_4;
                    DrawLine({0, yAlign},
                             {windowSize.width, yAlign},
                             context);
                }
                else
                {
                    if (prevY != INVALID_NEAREST_POSITION)
                    {
                        geo->pos.y = prevY;
                        prevY = INVALID_NEAREST_POSITION;
                    }
                }
            };
            yPointNegData.onEntityChanged = onEntityChanged;

            auto yPointNeg = ECSCreateEntity(
                "Y point Neg",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x, geo->pos.y + axisWidth + CENTER_SIZE / 2,
                        CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_0, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, 0, axisWidth + CENTER_SIZE),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &yPointNegData),
                });

            moveEntities.push_back(yPointNeg);
            ADD_ALL(yPointNeg);

            TransformScriptData leftTopData;
            leftTopData.entity = entityId;
            leftTopData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->relPos.x = -geo->size.width / 2 - CENTER_SIZE / 2;
                parent->relPos.y = -geo->size.height / 2 - CENTER_SIZE / 2;
            };
            leftTopData.onEntityChanged = onEntityChanged;

            leftTopData.onResizeMain =
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->size.width -= delta.x * 2;
                geo->size.height -= delta.y * 2;
            };

            auto leftTopPoint = ECSCreateEntity(
                "left-top-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x - geo->size.width / 2 - CENTER_SIZE / 2,
                        geo->pos.y - geo->size.height / 2 - CENTER_SIZE / 2,
                        CENTER_SIZE, CENTER_SIZE,
                        0.0f, PRIORITY_1, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent, entityId,
                        -geo->size.width / 2 - CENTER_SIZE / 2,
                        -geo->size.height / 2 - CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &leftTopData),
                });

            resizeEntities.push_back(leftTopPoint);
            ADD_ALL(leftTopPoint);

            TransformScriptData rightTopData;
            rightTopData.entity = entityId;
            rightTopData.onAddEntReset =
                [](
                    entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->relPos.x = geo->size.width / 2 + CENTER_SIZE / 2;
                parent->relPos.y = -geo->size.height / 2 - CENTER_SIZE / 2;
            };

            rightTopData.onResizeMain =
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->size.width += delta.x * 2;
                geo->size.height -= delta.y * 2;
            };

            rightTopData.onEntityChanged = onEntityChanged;

            auto rightTopPoint = ECSCreateEntity(
                "right-top-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x + geo->size.width / 2 + CENTER_SIZE / 2,
                        geo->pos.y - geo->size.height / 2 - CENTER_SIZE / 2,
                        CENTER_SIZE,
                        CENTER_SIZE, 0.0f,
                        PRIORITY_1,
                        NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        geo->size.width / 2 + CENTER_SIZE / 2,
                        -geo->size.height / 2 - CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &rightTopData),
                });

            resizeEntities.push_back(rightTopPoint);
            ADD_ALL(rightTopPoint);

            TransformScriptData rightBottomData;
            rightBottomData.entity = entityId;
            rightBottomData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->relPos.x = geo->size.width / 2 + CENTER_SIZE / 2;
                parent->relPos.y = geo->size.height / 2 + CENTER_SIZE / 2;
            };

            rightBottomData.onResizeMain =
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->size.width += delta.x * 2;
                geo->size.height += delta.y * 2;
            };
            rightBottomData.onEntityChanged = onEntityChanged;

            auto rightBottomPoint = ECSCreateEntity(
                "right-bottom-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x + geo->size.width / 2 + CENTER_SIZE / 2,
                        geo->pos.y + geo->size.height / 2 + CENTER_SIZE / 2,
                        CENTER_SIZE,
                        CENTER_SIZE, 0.0f,
                        PRIORITY_1,
                        NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        geo->size.width / 2 + CENTER_SIZE / 2,
                        geo->size.height / 2 + CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &rightBottomData),
                });

            resizeEntities.push_back(rightBottomPoint);
            ADD_ALL(rightBottomPoint);

            TransformScriptData leftBottomData;
            leftBottomData.entity = entityId;
            leftBottomData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->relPos.x = -geo->size.width / 2 - CENTER_SIZE / 2;
                parent->relPos.y = geo->size.height / 2 + CENTER_SIZE / 2;
            };

            leftBottomData.onResizeMain =
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->size.width -= delta.x * 2;
                geo->size.height += delta.y * 2;
            };
            leftBottomData.onEntityChanged = onEntityChanged;

            auto leftBottomPoint = ECSCreateEntity(
                "left-bottom-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x - geo->size.width / 2 - CENTER_SIZE / 2,
                        geo->pos.y + geo->size.height / 2 + CENTER_SIZE / 2,
                        CENTER_SIZE,
                        CENTER_SIZE, 0.0f,
                        PRIORITY_1,
                        NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        -geo->size.width / 2 - CENTER_SIZE / 2,
                        geo->size.height / 2 + CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &leftBottomData),
                });

            resizeEntities.push_back(leftBottomPoint);
            ADD_ALL(leftBottomPoint);

            TransformScriptData leftPointData;
            leftPointData.entity = entityId;
            leftPointData.onResizeMain =
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->size.width -= delta.x * 2;
            };

            leftPointData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->relPos.x = -geo->size.width / 2 - CENTER_SIZE / 2;
            };
            leftPointData.onEntityChanged = onEntityChanged;

            auto leftPoint = ECSCreateEntity(
                "left-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x - geo->size.width / 2 - CENTER_SIZE / 2,
                        geo->pos.y, CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_1, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        -geo->size.width / 2 - CENTER_SIZE / 2,
                        0),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &leftPointData),
                });

            resizeEntities.push_back(leftPoint);
            ADD_ALL(leftPoint);

            TransformScriptData rightPointData;
            rightPointData.entity = entityId;
            rightPointData.onResizeMain =
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->size.width += delta.x * 2;
            };

            rightPointData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->relPos.x = geo->size.width / 2 + CENTER_SIZE / 2;
            };
            rightPointData.onEntityChanged = onEntityChanged;

            auto rightPoint = ECSCreateEntity(
                "right-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x + geo->size.width / 2 + CENTER_SIZE / 2,
                        geo->pos.y, CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_1, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        geo->size.width / 2 + CENTER_SIZE / 2,
                        0),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &rightPointData),
                });

            resizeEntities.push_back(rightPoint);
            ADD_ALL(rightPoint);

            TransformScriptData topPointData;
            topPointData.entity = entityId;
            topPointData.onResizeMain =
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->size.height -= delta.y * 2;
            };

            topPointData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->relPos.y = -geo->size.height / 2 - CENTER_SIZE / 2;
            };
            topPointData.onEntityChanged = onEntityChanged;

            auto topPoint = ECSCreateEntity(
                "top-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x, geo->pos.y - geo->size.height / 2 - CENTER_SIZE / 2,
                        CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_1, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        0,
                        -geo->size.height / 2 - CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &topPointData),
                });

            resizeEntities.push_back(topPoint);
            ADD_ALL(topPoint);

            TransformScriptData bottomPointData;
            bottomPointData.entity = entityId;
            bottomPointData.onResizeMain =
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->size.height += delta.y * 2;
            };

            bottomPointData.onAddEntReset =
                [](entity_id_t entityId)
            {
                auto parent = ECS_GET_COMPONENT(entityId, Parent);
                auto geo = ECS_GET_COMPONENT(parent->parentId, Geometry);

                parent->relPos.y = geo->size.height / 2 + CENTER_SIZE / 2;
            };
            bottomPointData.onEntityChanged = onEntityChanged;

            auto bottomPoint = ECSCreateEntity(
                "bottom-point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x, geo->pos.y + geo->size.height / 2 + CENTER_SIZE / 2,
                        CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_1, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        0,
                        geo->size.height / 2 + CENTER_SIZE / 2),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &bottomPointData),
                });

            resizeEntities.push_back(bottomPoint);
            ADD_ALL(bottomPoint);

            TransformScriptData rotatePointData;
            rotatePointData.entity = entityId;
            rotatePointData.onResizeMain =
                [](
                    const Position delta,
                    f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->rotation += angularDelta;

                if (geo->rotation > 360.0f)
                {
                    geo->rotation -= 360.0f;
                }
                else if (geo->rotation < 0.0f)
                {
                    geo->rotation += 360.0f;
                }
            };
            rotatePointData.onEntityChanged = onEntityChanged;

            auto rotatePoint = ECSCreateEntity(
                "rotate point",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x + axisWidth + CENTER_SIZE / 2,
                        geo->pos.y,
                        CENTER_SIZE, CENTER_SIZE, 0.0f,
                        PRIORITY_1, NTT_RED),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId,
                        axisWidth + CENTER_SIZE / 2,
                        0),
                    ECS_CREATE_COMPONENT(Hovering),
                    ECS_CREATE_COMPONENT(
                        NativeScriptComponent,
                        "transform-script",
                        INVALID_OBJECT_ID,
                        &rotatePointData),
                });

            rotateEntities.push_back(rotatePoint);
            ADD_ALL(rotatePoint);

            auto xRotateAxis = ECSCreateEntity(
                "X Axis Rotate",
                {
                    ECS_CREATE_COMPONENT(
                        Geometry,
                        geo->pos.x + axisWidth / 2 + CENTER_SIZE / 2,
                        geo->pos.y, axisWidth, 3, 0.0f,
                        PRIORITY_0, NTT_GREEN),
                    ECS_CREATE_COMPONENT(
                        Parent,
                        entityId, axisWidth / 2 + CENTER_SIZE / 2, 0),
                });

            rotateEntities.push_back(xRotateAxis);
            ADD_ALL(xRotateAxis);

            ChangeMoveState(currentTool == MOVE);
            ChangeResizeState(currentTool == SCALE);
            ChangeRotateState(currentTool == ROTATE);

            ECSBeginLayer(GAME_LAYER);
        }

        void OnEditorChooseEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            auto entityId = context.u32_data[0];
            Clear();
            ChooseNewEntity(entityId);
        }

        void OnEditorAppendEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            auto entityId = context.u32_data[0];
            ChooseNewEntity(entityId);
        }

        void OnEditorClearChosenEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            Clear();
        }

        void OnEditorSelectedRequest(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            f32 deltaX = context.f32_data[0];
            f32 deltaY = context.f32_data[1];
            f32 x = context.f32_data[2];
            f32 y = context.f32_data[3];

            f32 angularDelta = 0.0f;

            if (selectedEntities.size() != 0)
            {
                auto geo = ECS_GET_COMPONENT(selectedEntities[0], Geometry);
                if (geo == nullptr)
                {
                    return;
                }

                angularDelta = atan2(y - geo->pos.y, x - geo->pos.x) -
                               atan2(y - deltaY - geo->pos.y, x - deltaX - geo->pos.x);

                angularDelta *= 180.0f / 3.14;

                if (angularDelta < 0)
                {
                    angularDelta += 360.0f;
                }
            }

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
                    onResizeMain({deltaX, deltaY}, angularDelta, geo);
                }
            }

            TriggerEvent(NTT_EDITOR_TRANSFORM_CHANGED, nullptr, context);
        }

        void OnSceneChanged(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            Clear();
            TriggerEvent(NTT_EDITOR_RESET_CAMERA);
        }

        void ChangeMoveState(b8 active)
        {
            PROFILE_FUNCTION();
            for (auto entityId : moveEntities)
            {
                ECSSetComponentActive(entityId, typeid(NativeScriptComponent), active);
                ECSSetComponentActive(entityId, typeid(Geometry), active);
            }
        }

        void ChangeRotateState(b8 active)
        {
            PROFILE_FUNCTION();
            for (auto entityId : rotateEntities)
            {
                ECSSetComponentActive(entityId, typeid(NativeScriptComponent), active);
                ECSSetComponentActive(entityId, typeid(Geometry), active);
            }
        }

        void ChangeResizeState(b8 active)
        {
            PROFILE_FUNCTION();
            for (auto entityId : resizeEntities)
            {
                ECSSetComponentActive(entityId, typeid(NativeScriptComponent), active);
                ECSSetComponentActive(entityId, typeid(Geometry), active);
            }
        }

        void OnToolTypeChanged(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            auto tool = static_cast<ToolType>(context.u8_data[0]);
            currentTool = tool;

            for (auto entityId : allDrawnEntities)
            {
                ChangeMoveState(currentTool == MOVE);
                ChangeResizeState(currentTool == SCALE);
                ChangeRotateState(currentTool == ROTATE);
            }
        }
    };

    EditorSystem::EditorSystem()
        : m(CreateScope<Impl>())
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

        m->selectedEntities.clear();
        m->drawnEntities.clear();

        ScriptStoreLoad(
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
                                m.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_CHOOSE_ENTITY,
                      std::bind(&Impl::OnEditorChooseEntity,
                                m.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_APPEND_ENTITY,
                      std::bind(&Impl::OnEditorAppendEntity,
                                m.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_CLEAR_CHOSEN_ENTITY,
                      std::bind(&Impl::OnEditorClearChosenEntity,
                                m.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_SELECTED_MOVE_REQUEST,
                      std::bind(&Impl::OnEditorSelectedRequest,
                                m.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(NTT_EDITOR_SELECTED_RESIZE_REQUEST,
                      std::bind(&Impl::OnEditorSelectedRequest,
                                m.get(),
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3));

        RegisterEvent(
            NTT_EDITOR_TRANSFORM_CHANGED_REQUEST,
            std::bind(&Impl::OnEditorSelectedRequest,
                      m.get(),
                      std::placeholders::_1,
                      std::placeholders::_2,
                      std::placeholders::_3));

        RegisterEvent(
            NTT_EDITOR_OPEN_SCENE,
            std::bind(&Impl::OnSceneChanged,
                      m.get(),
                      std::placeholders::_1,
                      std::placeholders::_2,
                      std::placeholders::_3));

        RegisterEvent(
            NTT_SCENE_CHANGED,
            [&](event_code_t code, void *sender, const EventContext &context)
            { m->Clear(); });

        RegisterEvent(NTT_EDITOR_TOOL_TYPE_CHANGED,
                      std::bind(&Impl::OnToolTypeChanged,
                                m.get(),
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

        if (CheckState(NTT_BUTTON_LEFT, NTT_UP) &&
            (m->prevX != INVALID_NEAREST_POSITION ||
             m->prevY != INVALID_NEAREST_POSITION))
        {
            m->prevX = INVALID_NEAREST_POSITION;
            m->prevY = INVALID_NEAREST_POSITION;
        }

        auto hoveredIds = GetHoveredTexture();

        if (hoveredIds.size() == 0)
        {
            return;
        }

        if (hoveredIds[hoveredIds.size() - 1] != entityId)
        {
            return;
        }

        if (m->allDrawnEntities.Contains(entityId))
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
            if (m->selectedEntities.Contains(entityId))
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
