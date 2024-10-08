#include <NTTEngine/editor/editor_system.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/renderer/renderer.hpp>
#include <NTTEngine/application/script_system/script_store.hpp>
#include <NTTEngine/application/script_system/script_component.hpp>

#include "controllers/MoveXController.hpp"
#include "controllers/MoveAroundController.hpp"

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

        RegisterEvent(NTT_LAYER_CHANGED,
                      std::bind(&Impl::OnLayerChanged,
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
            m_impl->Clear();
        }

        m_impl->selectedEntities.push_back(entityId);

        if (!m_impl->drawnEntities.Contains(entityId))
        {
            m_impl->drawnEntities[entityId] = List<entity_id_t>();
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
                    m_impl->moveAroundControllerScriptId,
                    INVALID_OBJECT_ID,
                    &entityId),
            });

        position_t axisWidth = 100;

        auto xAxis = ECSCreateEntity(
            "X Axis",
            {
                ECS_CREATE_COMPONENT(
                    Geometry,
                    geo->x + axisWidth / 2 + CENTER_SIZE / 2, geo->y, axisWidth, 3, 0.0f,
                    PRIORITY_0, NTT_GREEN),
            });

        m_impl->drawnEntities[entityId].push_back(center);
        m_impl->drawnEntities[entityId].push_back(xAxis);

        m_impl->allDrawnEntities.push_back(center);
        m_impl->allDrawnEntities.push_back(xAxis);

        ECSBeginLayer(GAME_LAYER);
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
