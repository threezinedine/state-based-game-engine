#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/resources/resources.hpp>
#include <NTTEngine/resources/resource_common.h>
#include <NTTEngine/core/auto_naming.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace ecs;
    using namespace memory;

    struct SceneInfo
    {
        String sceneName;
        String filePath;
        List<Ref<EntityInfo>> entities;

        JSON ToJSON() const;
        void FromJSON(const JSON &scene);

        List<ResourceInfo> GetResourceInfo();
        void SaveResourceInfo(List<ResourceInfo> resourcesInfo);

        void ReloadEntities();
        void SaveEntitiesInfo();
        void AddEntity();

    private:
        List<entity_id_t> m_entityIDs;
    };

    template <>
    inline String format(const String &str, const SceneInfo &info)
    {
        String newStr = str;
        newStr.Replace("{}", info.ToJSON().ToString(), FALSE);
        return newStr;
    }

    struct ProjectInfo
    {
        String name;
        String path;
        i32 width = 800;
        i32 height = 600;
        String title;
        String defaultResourceFile;
        Dictionary<String, Ref<SceneInfo>> scenes;

        ProjectInfo() = default;

        JSON ToJSON();
        void From(const JSON &project);
        String GetProjectFilePath();
        void AddNewScene(const String &sceneName);
        List<ResourceInfo> GetDefaultResourcesInfo();
        void SaveDefaultResources(List<ResourceInfo> resources);
        b8 ContainScene(Ref<SceneInfo> scene);
    };

    struct EditorConfig
    {
        String lastProjectPath;
        String lastProjectFile;
        f32 editorFontSize = 2.0f;

        EditorConfig() = default;

        JSON ToJSON() const;
        void From(const JSON &config);
        String GetConfigFilePath();
    };

    struct EditorData
    {
        Ref<ProjectInfo> project;
        Ref<SceneInfo> scene;
        Ref<EditorConfig> config;
    };
} // namespace ntt
