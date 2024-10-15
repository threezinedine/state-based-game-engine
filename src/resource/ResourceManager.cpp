#include <NTTEngine/resources/ResourceManager.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/resources/Resource.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

#include <NTTEngine/renderer/ImageResource.hpp>
#include <NTTEngine/audio/AudioResource.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/script_system/ScriptResource.hpp>

#include "ResourceTest.hpp"

namespace ntt
{
    namespace
    {
        Dictionary<String, Scope<Resource>> s_resources;
        Dictionary<String, resource_id_t> s_resourceIDs;

        String s_sourcePath = CurrentDirectory();
        String s_projectPath = "";
        b8 s_editorMode = FALSE;
    } // namespace

    void ResourceInit(b8 editorMode)
    {
        PROFILE_FUNCTION();

        s_projectPath = CurrentDirectory();
        s_sourcePath = CurrentDirectory();
        s_resources.clear();
        s_editorMode = editorMode;
    }

    void ResourceLoad(List<ResourceInfo> infos)
    {
        PROFILE_FUNCTION();

        for (auto info : infos)
        {
            if (s_resourceIDs.Contains(info.name))
            {
                continue;
            }

            Scope<Resource> resource;
            info.addintionalInfo.Set("sourcePath", s_sourcePath);
            info.addintionalInfo.Set("projectPath", s_projectPath);
            info.addintionalInfo.Set("editorMode", s_editorMode);

            // =============== Begin of handling the incoming resource ===============
            switch (info.type)
            {
            case ResourceType::AUDIO:
                resource = CreateScope<AudioResource>(info);
                break;
            case ResourceType::IMAGE:
                resource = CreateScope<ImageResource>(info);
                break;
            case ResourceType::SCRIPT:
                resource = CreateScope<ScriptResource>(info);
                break;
            default:
                resource = CreateScope<ResourceTest>(info);
            }
            // =============== End of handling the incoming resource ===============

            s_resources[info.name] = std::move(resource);
            s_resourceIDs[info.name] = s_resources[info.name]->Load();
        }
    }

    void ResourceUnload(List<ResourceInfo> infos)
    {
        PROFILE_FUNCTION();

        for (auto info : infos)
        {
            if (!s_resourceIDs.Contains(info.name))
            {
                continue;
            }

            s_resources[info.name]->Unload();
            s_resources.erase(info.name);
            s_resourceIDs.erase(info.name);
        }
    }

    List<ResourceInfo> ExtractInfoFromJSON(const JSON &config)
    {
        PROFILE_FUNCTION();

        List<ResourceInfo> infos;

        auto keys = config.GetKeys();

        List<JSON> resources = config.ToList();

        for (auto resource : resources)
        {
            ResourceInfo resourceInfo;
            resourceInfo.From(resource);

            infos.push_back(resourceInfo);
        }

        return infos;
    }

    void Resource_SetProjectPath(const String &projectPath)
    {
        PROFILE_FUNCTION();

        if (!IsExist(projectPath))
        {
            NTT_ENGINE_WARN("The project path {} is not exist.", projectPath);
            return;
        }

        s_projectPath = projectPath;
    }

    resource_id_t GetResourceID(const String &name)
    {
        PROFILE_FUNCTION();

        if (s_resourceIDs.Contains(name))
        {
            return s_resourceIDs[name];
        }

        return INVALID_RESOURCE_ID;
    }

    String GetResourceName(resource_id_t id)
    {
        PROFILE_FUNCTION();

        for (auto &resource : s_resourceIDs)
        {
            if (resource.second == id)
            {
                return resource.first;
            }
        }

        return "Not found";
    }

    void ResourceShutdown()
    {
        PROFILE_FUNCTION();

        for (auto &resource : s_resources)
        {
            resource.second->Unload();
        }

        s_resources.clear();
        s_resourceIDs.clear();
    }
} // namespace ntt
