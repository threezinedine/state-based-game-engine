#include <NTTEngine/application/scene_system/scene_system.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/resources/ResourceManager.hpp>

namespace ntt
{
    using namespace log;
    using namespace event;
    using namespace ecs;

    namespace
    {
        Dictionary<String, std::function<void()>> s_scenes;
        List<entity_id_t> s_entities;

        String s_currentScene = "";

        void CreateEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            entity_id_t id = context.u32_data[0];

            if (s_entities.Contains(id))
            {
                NTT_ENGINE_TRACE("The entity {0} is already created", id);
                return;
            }

            s_entities.push_back(id);
        }

        void DeleteEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            entity_id_t id = context.u32_data[0];

            s_entities.RemoveItem(id);
        }
    } // namespace

    void SceneInit(List<std::pair<String, std::function<void()>>> scenes)
    {
        PROFILE_FUNCTION();
        if (scenes.size() == 0)
        {
            NTT_ENGINE_ERROR("No scene is found, the game can not run.");
            return;
        }

        s_currentScene = scenes[0].first;

        RegisterEvent(NTT_ENTITY_CREATED, CreateEntity);
        RegisterEvent(NTT_ENTITY_DESTROYED, DeleteEntity);

        for (auto &scene : scenes)
        {
            s_scenes[scene.first] = scene.second;
        }

        s_entities.clear();

        ChangeScene(s_currentScene);
        s_scenes[s_currentScene]();
    }

    void SceneOpen(const String &sceneName)
    {
        PROFILE_FUNCTION();

        if (!s_scenes.Contains(sceneName))
        {
            NTT_ENGINE_WARN("The scene {0} is not found", sceneName);
            return;
        }

        if (s_currentScene == sceneName)
        {
            NTT_ENGINE_WARN("The scene {0} is already opened", sceneName);
            return;
        }

        // delete all entities and components of the current scene
        for (auto &entityId : s_entities)
        {
            ECSDeleteEntity(entityId);
        }

        s_currentScene = sceneName;
        ChangeScene(s_currentScene);
        s_scenes[s_currentScene]();

        EventContext context;
        TriggerEvent(NTT_SCENE_CHANGED, nullptr, context);
    }

    void SceneShutdown()
    {
        PROFILE_FUNCTION();

        s_entities.clear();
        s_scenes.clear();
    }
} // namespace ntt
