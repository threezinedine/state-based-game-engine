#include <NTTEngine/application/layer_system/layer_system.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/renderer/renderer.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    using namespace ecs;
    using namespace log;
    using namespace renderer;
    using namespace event;

#define LAYER_RANGE 5

    namespace
    {
        List<List<entity_id_t>> layers;
        List<b8> layersVisibility;
        layer_t currentLayer = GAME_LAYER;
        layer_t currentRunningLayer = GAME_LAYER;
        layer_t preLayer = GAME_LAYER;

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

            if (layers.size() <= currentLayer)
            {
                return;
            }

            if (layers[currentLayer].Contains(id))
            {
                return;
            }

            layers[currentLayer].push_back(id);

            if (currentLayer != currentRunningLayer)
            {
                auto texture = ECS_GET_COMPONENT(id, Texture);

                if (texture != nullptr)
                {
                    texture->priority += currentRunningLayer * LAYER_RANGE;
                }

                auto text = ECS_GET_COMPONENT(id, Text);

                if (text != nullptr)
                {
                    text->priority += currentRunningLayer * LAYER_RANGE;
                }

                ECSGetEntity(id)->active = FALSE;
            }
        }

        void EntityDestroyedCallback(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            auto id = context.u32_data[0];

            for (auto &layer : layers)
            {
                layer.RemoveItem(id);
            }
        }
    } // namespace

    void LayerInit()
    {
        PROFILE_FUNCTION();
        layers.clear();

        layers.push_back(List<entity_id_t>());
        layersVisibility.push_back(TRUE);

        currentLayer = GAME_LAYER;
        currentRunningLayer = GAME_LAYER;

        RegisterEvent(NTT_ENTITY_CREATED, EntityCreatedCallback);
        RegisterEvent(NTT_ENTITY_DESTROYED, EntityDestroyedCallback);

        RegisterEvent(NTT_DEBUG_BREAK, DebuggingCallback);
        RegisterEvent(NTT_DEBUG_CONTINUE, DebuggingContinueCallback);
    }

    void BeginLayer(layer_t layer)
    {
        PROFILE_FUNCTION();
        while (layers.size() <= layer)
        {
            layers.push_back(List<entity_id_t>());
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
        if (layers.size() <= layer)
        {
            return;
        }

        // turn off the drawing of the current layer
        for (auto &entity : layers[currentRunningLayer])
        {
            auto entityInfo = ECSGetEntity(entity);

            if (entityInfo == nullptr)
            {
                continue;
            }

            entityInfo->active = FALSE;
        }

        currentRunningLayer = layer;

        // turn on the drawing of the new layer
        for (auto &entity : layers[currentRunningLayer])
        {
            auto entityInfo = ECSGetEntity(entity);
            if (entityInfo == nullptr)
            {
                continue;
            }
            entityInfo->active = TRUE;
        }

        EventContext context;
        context.u16_data[0] = currentRunningLayer;
        TriggerEvent(NTT_LAYER_CHANGED, nullptr, context);
    }

    const List<entity_id_t> DrawnEntities()
    {
        PROFILE_FUNCTION();
        List<entity_id_t> entities;

        for (auto i = 0; i <= currentRunningLayer; i++)
        {
            entities.insert(entities.end(), layers[i].begin(), layers[i].end());
        }

        return entities;
    }

    const List<entity_id_t> UpdatedEntities()
    {
        PROFILE_FUNCTION();
        List<entity_id_t> entities;

        for (auto entity : layers[currentRunningLayer])
        {
            entities.push_back(entity);
        }

        return entities;
    }

    void LayerShutdown()
    {
        PROFILE_FUNCTION();
        layers.clear();
    }
}