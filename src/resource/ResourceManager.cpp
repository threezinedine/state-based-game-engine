#include <NTTEngine/resources/ResourceManager.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/resources/Resource.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

#include <NTTEngine/renderer/ImageResource.hpp>
#include <NTTEngine/audio/AudioResource.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/core/profiling.hpp>

#include "ResourceTest.hpp"

namespace ntt
{
    using namespace log;
    using namespace renderer;
    using namespace audio;
    using namespace event;

    namespace
    {
        b8 s_initialized = FALSE;

        // All resouces objects which are not the default
        //      resources and can be loaded and unloaded and the
        //      key is the scene name.
        Dictionary<String, std::vector<Scope<Resource>>> s_resources;

        // The dictionary which stores the resource name and its ID
        //      if the change scene is called, then the resources
        //      will be reset and new resources will be loaded.
        //      The key is the resource name.
        Dictionary<String, resource_id_t> s_resourcesDictionary;

        // These default resources will be loaded from the beginning
        //      and will be unloaded when the program is finished.
        //      The key is the resource name.
        Dictionary<String, resource_id_t> s_defaultResourcesDict;

        std::vector<Scope<Resource>> s_defaultResourcesObjects;
#define EMPTY_SCENE ""
        String s_currentScene = EMPTY_SCENE;

        void LoadSceneResources(const String &sceneName)
        {
            PROFILE_FUNCTION();
            auto &sceneResources = s_resources[sceneName];

            for (auto &resource : sceneResources)
            {
                s_resourcesDictionary[resource->GetName()] = resource->Load();
            }
        }

        void UnloadCurrentScene()
        {
            PROFILE_FUNCTION();
            s_resourcesDictionary.clear();

            for (auto &resource : s_resources[s_currentScene])
            {
                resource->Unload();
            }
        }

    } // namespace

    void ResourceInit()
    {
        PROFILE_FUNCTION();
        if (s_initialized)
        {
            NTT_ENGINE_WARN("Resource manager is already initialized.");
            return;
        }

        s_resources.clear();
        s_resourcesDictionary.clear();
        s_defaultResourcesDict.clear();
        s_defaultResourcesObjects.clear();
        s_currentScene = EMPTY_SCENE;

        s_initialized = TRUE;
    }

    void RegisterResource(const String &sceneName, const ResourceInfo &info)
    {
        PROFILE_FUNCTION();
        if (!s_initialized)
        {
            NTT_ENGINE_WARN("Resource manager is not initialized.");
            return;
        }

        Scope<Resource> resource = nullptr;

        // =============== Begin of handling the incoming resource ===============
        switch (info.type)
        {
        case ResourceType::AUDIO:
            resource = CreateScope<AudioResource>(info);
            break;
        case ResourceType::IMAGE:
            resource = CreateScope<ImageResource>(info);
            break;
        default:
            resource = CreateScope<ResourceTest>(info);
        }
        // =============== End of handling the incoming resource ===============

        if (sceneName == "default")
        {
            if (s_defaultResourcesDict.Contains(info.name))
            {
                NTT_ENGINE_WARN("The default resource {} is already loaded.", info.name);
                return;
            }

            s_defaultResourcesDict[info.name] = resource->Load();
            s_defaultResourcesObjects.push_back(std::move(resource));
        }
        else
        {
            if (!s_resources.Contains(sceneName))
            {
                s_resources[sceneName] = std::vector<Scope<Resource>>();
            }

            s_resources[sceneName].push_back(std::move(resource));
        }
    }

    void ResourceLoadConfig(const JSON &config)
    {
        PROFILE_FUNCTION();
        if (!s_initialized)
        {
            NTT_ENGINE_WARN("Resource manager is not initialized.");
            return;
        }

        auto keys = config.GetKeys();

        for (auto sceneName : keys)
        {
            List<JSON> resources = config.GetList<JSON>(sceneName);

            for (auto resource : resources)
            {
                ResourceInfo resourceInfo;

                if (!resource.Contains<String>("name"))
                {
                    break;
                }

                if (!(resource.Contains<String>("path") || resource.Contains<String>("relPath")))
                {
                    break;
                }

                if (!resource.Contains<u32>("type"))
                {
                    break;
                }

                resourceInfo.name = resource.Get<String>("name");

                if (resource.Contains<String>("relPath"))
                {
                    resourceInfo.path = RelativePath(resource.Get<String>("relPath"));
                }
                else
                {
                    resourceInfo.path = resource.Get<String>("path");
                }

                resourceInfo.type = static_cast<ResourceType>(resource.Get<u32>("type"));
                resourceInfo.addintionalInfo = resource;

                RegisterResource(sceneName, resourceInfo);
            }
        }
    }

    void ChangeScene(const String &sceneName)
    {
        PROFILE_FUNCTION();
        if (!s_initialized)
        {
            NTT_ENGINE_WARN("Resource manager is not initialized.");
            return;
        }

        if (s_currentScene == sceneName)
        {
            NTT_ENGINE_WARN("The scene is already loaded.");
            return;
        }

        if (!s_resources.Contains(sceneName) || sceneName == EMPTY_SCENE)
        {
            NTT_ENGINE_WARN("The scene {} is not found.", sceneName);
            return;
        }

        UnloadCurrentScene();
        s_currentScene = sceneName;
        LoadSceneResources(sceneName);
        return;
    }

    resource_id_t GetResourceID(const String &name)
    {
        PROFILE_FUNCTION();
        if (!s_initialized)
        {
            NTT_ENGINE_WARN("Resource manager is not initialized.");
            return RESOURCE_ID_DEFAULT;
        }

        if (s_defaultResourcesDict.Contains(name))
        {
            return s_defaultResourcesDict[name];
        }

        if (s_resourcesDictionary.Contains(name))
        {
            return s_resourcesDictionary[name];
        }

        return RESOURCE_ID_DEFAULT;
    }

    void ResourceShutdown()
    {
        PROFILE_FUNCTION();
        if (!s_initialized)
        {
            NTT_ENGINE_WARN("Resource manager is not initialized.");
            return;
        }

        if (s_currentScene != EMPTY_SCENE && s_resources.Contains(s_currentScene))
        {
            UnloadCurrentScene();
        }

        for (auto &resource : s_defaultResourcesObjects)
        {
            resource->Unload();
        }

        s_resources.clear();
        s_resourcesDictionary.clear();
        s_defaultResourcesObjects.clear();
        s_defaultResourcesDict.clear();
        s_initialized = FALSE;
    }
} // namespace ntt
