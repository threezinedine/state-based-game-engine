#include <NTTEngine/application/layer_system/layer_system.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/renderer/renderer.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/assertion.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>

namespace ntt
{
    using namespace ecs;
    using namespace log;
    using namespace renderer;
    using namespace event;
    using namespace memory;

#define INVALID_UI_LAYER 255

    namespace
    {
        Scope<List<entity_id_t>> layers[MAX_LAYERS];
        List<b8> layersVisibility;
        layer_t currentLayer = GAME_LAYER;
        layer_t currentRunningLayer = GAME_LAYER;
        layer_t preLayer = GAME_LAYER;

        u8 uiLayerVisible = INVALID_UI_LAYER;

        void DebuggingCallback(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();

            preLayer = currentLayer;
            LayerMakeVisible(DEBUG_LAYER);
        }

        void DebuggingContinueCallback(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();

            auto isOffPermanent = context.b8_data[0];

            if (isOffPermanent)
            {
                return;
            }

            LayerMakeVisible(preLayer);
        }

        void EntityCreatedCallback(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            auto id = context.u32_data[0];

            if (currentLayer > MAX_LAYERS)
            {
                return;
            }

            if (layers[currentLayer] == nullptr)
            {
                layers[currentLayer] = CreateScope<List<entity_id_t>>();
            }

            if (layers[currentLayer]->Contains(id))
            {
                return;
            }

            layers[currentLayer]->push_back(id);

            auto texture = ECS_GET_COMPONENT(id, Texture);

            if (texture != nullptr)
            {
                texture->priority += (currentLayer * LAYER_PRIORITY_RANGE);
            }

            auto text = ECS_GET_COMPONENT(id, Text);

            if (text != nullptr)
            {
                text->priority += (currentLayer * LAYER_PRIORITY_RANGE);
            }

            if (currentLayer != currentRunningLayer)
            {
                ECSGetEntity(id)->active = FALSE;
            }
        }

        void EntityDestroyedCallback(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            auto id = context.u32_data[0];

            for (auto &layer : layers)
            {
                if (layer == nullptr)
                {
                    continue;
                }
                layer->RemoveItem(id);
            }
        }
    } // namespace

    void LayerInit()
    {
        PROFILE_FUNCTION();
        memset(layers, 0, sizeof(layers));

        for (auto i = 0; i < MAX_LAYERS; i++)
        {
            layers[i] = CreateScope<List<entity_id_t>>();
        }

        currentLayer = GAME_LAYER;
        currentRunningLayer = GAME_LAYER;
        uiLayerVisible = INVALID_UI_LAYER;

        RegisterEvent(NTT_ENTITY_CREATED, EntityCreatedCallback);
        RegisterEvent(NTT_ENTITY_DESTROYED, EntityDestroyedCallback);

        RegisterEvent(NTT_DEBUG_BREAK, DebuggingCallback);
        RegisterEvent(NTT_DEBUG_CONTINUE, DebuggingContinueCallback);
    }

    void BeginLayer(layer_t layer)
    {
        PROFILE_FUNCTION();

        if (MAX_LAYERS <= layer)
        {
            NTT_ENGINE_WARN("The layer {} is out of range", layer);
            return;
        }

        currentLayer = layer;
    }

    void LayerUpdate(f32 deltaTime)
    {
        PROFILE_FUNCTION();
    }

    void LayerMakeVisible(layer_t layer)
    {
        PROFILE_FUNCTION();
        if (MAX_LAYERS <= layer)
        {
            NTT_ENGINE_WARN("The layer {} is out of range", layer);
            return;
        }

        if (GAME_LAYER < layer && layer < DEBUG_LAYER)
        {
            uiLayerVisible = layer;
        }

        if (layer == GAME_LAYER)
        {
            uiLayerVisible = INVALID_UI_LAYER;
        }

        currentRunningLayer = layer;

        EventContext context;
        context.u16_data[0] = currentRunningLayer;
        TriggerEvent(NTT_LAYER_CHANGED, nullptr, context);
    }

    const List<entity_id_t> DrawnEntities()
    {
        PROFILE_FUNCTION();
        List<entity_id_t> entities;

        entities.insert(
            entities.end(),
            layers[GAME_LAYER]->begin(),
            layers[GAME_LAYER]->end());

        if (uiLayerVisible != INVALID_UI_LAYER)
        {
            entities.insert(
                entities.end(),
                layers[uiLayerVisible]->begin(),
                layers[uiLayerVisible]->end());
        }

        if (currentRunningLayer == DEBUG_LAYER)
        {
            entities.insert(
                entities.end(),
                layers[DEBUG_LAYER]->begin(),
                layers[DEBUG_LAYER]->end());
        }

        return entities;
    }

    const List<entity_id_t> UpdatedEntities()
    {
        PROFILE_FUNCTION();
        List<entity_id_t> entities;

        for (auto entity : *(layers[currentRunningLayer]))
        {
            entities.push_back(entity);
        }

        return entities;
    }

    void LayerShutdown()
    {
        PROFILE_FUNCTION();

        for (auto &layer : layers)
        {
            layer.reset();
            ASSERT_M(layer == nullptr, "The layer is not reset properly");
        }
    }
}