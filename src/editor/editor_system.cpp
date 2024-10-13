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
#include <math.h>

#include "controllers/TransformScript.hpp"
#include "editor_tool/editor_tool.hpp"

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
        List<entity_id_t> rotateEntities;
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
                    rotateEntities.RemoveItem(btnId);
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

#define ADD_ALL(entity)                        \
    drawnEntities[entityId].push_back(entity); \
    allDrawnEntities.push_back(entity);        \
    // NTT_ENGINE_DEBUG("Entity {} is added with the id {}", #entity, entity);     \
    // NTT_ENGINE_DEBUG("Drawn entities: {}", drawnEntities[entityId].ToString()); \
    // NTT_ENGINE_DEBUG("All drawn entities: {}", allDrawnEntities.ToString());    \
    // NTT_ENGINE_DEBUG("Move entities: {}", moveEntities.ToString());             \
    // NTT_ENGINE_DEBUG("Resize entities: {}", resizeEntities.ToString());         \
    // NTT_ENGINE_DEBUG("Rotate entities: {}", rotateEntities.ToString());
            TransformScriptData centerData;
            centerData.entity = entityId;
            centerData.onResizeMain =
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->pos.x += delta.x;
                geo->pos.y += delta.y;
            };
            centerData.onEntityChanged =
                [](const Position &start, const Position &end)
            {
                TriggerEvent(NTT_EDITOR_SAVE_SCENE);
            };

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
                        transformScriptId,
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
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->pos.x += delta.x;
            };

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
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &xPointData),
                });

            moveEntities.push_back(xPoint);
            ADD_ALL(xPoint);

            TransformScriptData xPointNegData;
            xPointNegData.entity = entityId;
            xPointNegData.onResizeMain =
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->pos.x += delta.x;
            };

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
                        transformScriptId,
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
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->pos.y += delta.y;
            };

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
                        transformScriptId,
                        INVALID_OBJECT_ID,
                        &yPointData),
                });

            moveEntities.push_back(yPoint);
            ADD_ALL(yPoint);

            TransformScriptData yPointNegData;
            yPointNegData.entity = entityId;
            yPointNegData.onResizeMain =
                [](
                    const Position delta,
                    const f32 angularDelta,
                    Ref<Geometry> geo)
            {
                geo->pos.y += delta.y;
            };

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
                        transformScriptId,
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
                        transformScriptId,
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
                        transformScriptId,
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
                        transformScriptId,
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
                        transformScriptId,
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
                        transformScriptId,
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
                        transformScriptId,
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
                        transformScriptId,
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
                        transformScriptId,
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
                        transformScriptId,
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

            drawnEntities[entityId].push_back(center);
            drawnEntities[entityId].push_back(xAxis);
            drawnEntities[entityId].push_back(xPoint);
            drawnEntities[entityId].push_back(yAxis);
            drawnEntities[entityId].push_back(yPoint);
            drawnEntities[entityId].push_back(xAxisNeg);
            drawnEntities[entityId].push_back(xPointNeg);
            drawnEntities[entityId].push_back(yAxisNeg);
            drawnEntities[entityId].push_back(yPointNeg);

            drawnEntities[entityId].push_back(leftTopPoint);
            drawnEntities[entityId].push_back(rightTopPoint);
            drawnEntities[entityId].push_back(leftBottomPoint);
            drawnEntities[entityId].push_back(rightBottomPoint);
            drawnEntities[entityId].push_back(leftPoint);
            drawnEntities[entityId].push_back(rightPoint);
            drawnEntities[entityId].push_back(topPoint);
            drawnEntities[entityId].push_back(bottomPoint);

            drawnEntities[entityId].push_back(rotatePoint);
            drawnEntities[entityId].push_back(xRotateAxis);

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

            rotateEntities.push_back(rotatePoint);
            rotateEntities.push_back(xRotateAxis);

            // allDrawnEntities.push_back(center);
            // allDrawnEntities.push_back(xAxis);
            // allDrawnEntities.push_back(yAxis);
            // allDrawnEntities.push_back(xPoint);
            // allDrawnEntities.push_back(yPoint);
            // allDrawnEntities.push_back(leftTopPoint);
            // allDrawnEntities.push_back(rightTopPoint);
            // allDrawnEntities.push_back(leftBottomPoint);
            // allDrawnEntities.push_back(rightBottomPoint);
            // allDrawnEntities.push_back(xAxisNeg);
            // allDrawnEntities.push_back(xPointNeg);
            // allDrawnEntities.push_back(yAxisNeg);
            // allDrawnEntities.push_back(yPointNeg);
            // allDrawnEntities.push_back(leftPoint);
            // allDrawnEntities.push_back(rightPoint);
            // allDrawnEntities.push_back(topPoint);
            // allDrawnEntities.push_back(bottomPoint);
            // allDrawnEntities.push_back(rotatePoint);
            // allDrawnEntities.push_back(xRotateAxis);

            ChangeMoveState(currentTool == MOVE);
            ChangeResizeState(currentTool == SCALE);
            ChangeRotateState(currentTool == ROTATE);

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

        void ChangeMoveState(b8 active)
        {
            for (auto entityId : moveEntities)
            {
                ECSSetComponentActive(entityId, typeid(NativeScriptComponent), active);
                ECSSetComponentActive(entityId, typeid(Geometry), active);
            }
        }

        void ChangeRotateState(b8 active)
        {
            for (auto entityId : rotateEntities)
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
                ChangeRotateState(currentTool == ROTATE);
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
