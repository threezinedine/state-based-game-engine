#include <NTTEngine/application/layer_system/layer_system.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/renderer/renderer.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

namespace ntt
{
    using namespace ecs;
    using namespace log;
    using namespace renderer;
    using namespace event;

    namespace
    {
        Dictionary<LayerType, bool> layerVisibility;
        Dictionary<LayerType, List<entity_id_t>> layerEntities;
        LayerType currentLayer = LayerType::GAME_LAYER;
        LayerType currentRunningLayer = LayerType::GAME_LAYER;

        void EntityCreatedCallback(event_code_t code, void *sender, const EventContext &context)
        {
            auto id = context.u32_data[0];

            NTT_ENGINE_DEBUG("Current Layer: {} - Current running: {}",
                             (u8)currentLayer,
                             (u8)currentRunningLayer);

            if (!layerEntities.Contains(currentLayer))
            {
                return;
            }

            layerEntities[currentLayer].push_back(id);

            if (currentLayer != currentRunningLayer)
            {
                ECSSetEntityState(id, FALSE);
            }
        }
    } // namespace

    void LayerInit()
    {
        layerVisibility.clear();
        layerEntities.clear();

        layerVisibility[LayerType::GAME_LAYER] = true;
        layerEntities[LayerType::GAME_LAYER] = List<entity_id_t>();

        RegisterEvent(NTT_ENTITY_CREATED, EntityCreatedCallback);
    }

    void BeginLayer(LayerType type)
    {
        if (!layerEntities.Contains(type))
        {
            layerVisibility[type] = true;
            layerEntities[type] = List<entity_id_t>();
        }

        currentLayer = type;
    }

    void LayerUpdate(f32 deltaTime)
    {
    }

    void LayerMakeVisible(LayerType type)
    {
        if (!layerVisibility.Contains(type))
        {
            NTT_ENGINE_WARN("The layer is not created yet");
            return;
        }

        if (layerEntities.Contains(type))
        {
            // turn off the drawing of the current layer
            for (auto &entity : layerEntities[currentRunningLayer])
            {
                ECSSetEntityState(entity, FALSE);
            }

            currentRunningLayer = type;

            // turn on the drawing of the new layer
            for (auto &entity : layerEntities[currentRunningLayer])
            {
                ECSSetEntityState(entity, TRUE);
            }
            return;
        }
    }

    void LayerShutdown()
    {
        layerVisibility.clear();
        layerEntities.clear();
    }
}