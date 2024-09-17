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

    namespace
    {
        List<List<entity_id_t>> layers;
        layer_t currentLayer = GAME_LAYER;
        layer_t currentRunningLayer = GAME_LAYER;

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

        RegisterEvent(NTT_ENTITY_CREATED, EntityCreatedCallback);
        RegisterEvent(NTT_ENTITY_DESTROYED, EntityDestroyedCallback);
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
    }

    const List<entity_id_t> DrawedEntities()
    {
        PROFILE_FUNCTION();
        List<entity_id_t> entities;

        for (auto i = 0; i <= currentRunningLayer; i++)
        {
            entities.insert(entities.end(), layers[i].begin(), layers[i].end());
        }

        return entities;
    }

    void LayerShutdown()
    {
        PROFILE_FUNCTION();
        layers.clear();
    }
}