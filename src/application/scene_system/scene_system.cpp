#include <NTTEngine/application/scene_system/scene_system.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/ecs/ecs.hpp>

namespace ntt
{
    using namespace log;
    using namespace event;
    using namespace ecs;

    namespace
    {
        Dictionary<String, SceneContext> s_scenes;
        List<entity_id_t> s_entities;
        u16 s_currentLayer = 0;

        String s_currentScene = "";
        std::function<void(const String &)> s_onSceneChanged = nullptr;

        void OnSceneChanged(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            s_currentLayer = context.u16_data[0];
        }

        void CreateEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();

            if (s_currentLayer != GAME_LAYER)
            {
                return;
            }

            entity_id_t id = context.u32_data[0];

            if (s_entities.Contains(id))
            {
                NTT_ENGINE_TRACE("The entity {} is already created", id);
                return;
            }

            s_entities.push_back(id);
        }

        void DeleteEntity(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();

            if (s_currentLayer != GAME_LAYER)
            {
                return;
            }

            entity_id_t id = context.u32_data[0];

            s_entities.RemoveItem(id);
        }
    } // namespace

    void SceneInit(List<std::pair<String, SceneContext>> scenes,
                   std::function<void(const String &)> onSceneChanged)
    {
        PROFILE_FUNCTION();
        if (scenes.size() == 0)
        {
            NTT_ENGINE_ERROR("No scene is found, the game can not run.");
            return;
        }

        s_onSceneChanged = onSceneChanged;

        RegisterEvent(NTT_ENTITY_CREATED, CreateEntity);
        RegisterEvent(NTT_ENTITY_DESTROYED, DeleteEntity);

        RegisterEvent(NTT_DEFINED_LAYER_CHANGED, OnSceneChanged);

        s_scenes.clear();
        s_currentScene = "";

        for (auto &scene : scenes)
        {
            s_scenes[scene.first] = scene.second;
        }

        s_entities.clear();

        SceneOpen(scenes[0].first);
    }

    void SceneOpen(const String &sceneName)
    {
        PROFILE_FUNCTION();

        if (!s_scenes.Contains(sceneName))
        {
            NTT_ENGINE_WARN("The scene {} is not found", sceneName);
            return;
        }

        if (s_currentScene == sceneName)
        {
            NTT_ENGINE_WARN("The scene {} is already opened", sceneName);
            return;
        }

        // delete all entities and components of the current scene
        for (auto &entityId : s_entities)
        {
            ECSDeleteEntity(entityId);
        }

        if (s_scenes[s_currentScene].onExit != nullptr)
        {
            s_scenes[s_currentScene].onExit();
        }

        s_currentScene = sceneName;

        EventContext context;
        TriggerEvent(NTT_SCENE_CHANGED, nullptr, context);

        if (s_onSceneChanged != nullptr)
        {
            s_onSceneChanged(sceneName);
        }

        if (s_scenes[s_currentScene].createFunc != nullptr)
        {
            s_scenes[s_currentScene].createFunc();
        }
    }

    void SceneReload()
    {
        PROFILE_FUNCTION();

        for (auto &entityId : s_entities)
        {
            ECSDeleteEntity(entityId);
        }

        if (s_scenes[s_currentScene].onExit != nullptr)
        {
            s_scenes[s_currentScene].onExit();
        }

        if (s_scenes[s_currentScene].createFunc != nullptr)
        {
            s_scenes[s_currentScene].createFunc();
        }
    }

    void SceneShutdown()
    {
        PROFILE_FUNCTION();

        s_entities.clear();
        s_scenes.clear();
    }
} // namespace ntt
