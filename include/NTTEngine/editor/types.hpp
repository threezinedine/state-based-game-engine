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
    struct SceneInfo
    {
        String sceneName;
        String filePath;
        List<Ref<EntityInfo>> entities;
        List<ResourceInfo> resources;

        JSON ToJSON();
        void FromJSON(const JSON &scene);

        void ReloadResourceInfo();
        void SaveResourceInfo();

        void RemoveAllEntities();
        void ReloadEntities();
        void SaveEntitiesInfo();
        void AddEntity();

    private:
        List<entity_id_t> m_entityIDs;
    };

    struct ProjectInfo
    {
        String name;
        String path;
        i32 width = 800;
        i32 height = 600;
        String title;
        String defaultResourceFile;
        String defaultSceneName;
        Dictionary<String, Ref<SceneInfo>> scenes;
        List<ResourceInfo> defaultResources;

        ProjectInfo() = default;

        JSON ToJSON();
        void From(const JSON &project);
        String GetProjectFilePath();

        void AddNewScene(const String &sceneName);

        void ReloadDefaultResourcesInfo();
        void SaveDefaultResources();

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
