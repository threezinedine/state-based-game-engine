#pragma once

#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/resources/resources.hpp>
#include <NTTEngine/resources/resource_common.h>

namespace ntt
{
    struct SceneInfo
    {
        String sceneName;
        String filePath;

        JSON ToJSON() const
        {
            JSON scene = JSON("{}");
            scene.Set("sceneName", sceneName);
            scene.Set("filePath", filePath);
            return scene;
        }

        void FromJSON(const JSON &scene)
        {
            sceneName = scene.Get<String>("sceneName");
            filePath = scene.Get<String>("filePath");
        }

        List<ResourceInfo> GetResourceInfo()
        {
            List<ResourceInfo> resourcesInfo;
            JSON config = ReadFile(filePath);

            List<JSON> resourcesCfg = config.GetList<JSON>("resources");

            for (auto &resource : resourcesCfg)
            {
                ResourceInfo info;
                info.From(resource);
                resourcesInfo.push_back(info);
            }

            return resourcesInfo;
        }

        void SaveResourceInfo(List<ResourceInfo> resourcesInfo)
        {
            JSON config = ReadFile(filePath);
            List<JSON> resourceJSONs =
                resourcesInfo.Map<JSON>([](const ResourceInfo &info, ...) -> JSON
                                        { return info.ToJSON(); });

            config.Set("resources", resourceJSONs);

            OpenFile(filePath);
            Write(config.ToString());
            CloseFile();
        }
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

        void From(const JSON &project)
        {
            name = project.Get<String>("name");
            path = project.Get<String>("path");
            width = project.Get<i32>("width", 800);
            height = project.Get<i32>("height", 600);
            title = project.Get<String>("title");
            defaultResourceFile = project.Get<String>("defaultResourceFile");

            List<String> sceneNames = project.GetList<String>("sceneNames");
            scenes.clear();

            for (auto sceneName : sceneNames)
            {
                Ref<SceneInfo> scene = CreateRef<SceneInfo>();
                scene->sceneName = sceneName;
                scene->filePath = JoinPath({path, "scenes", format("{}.json", sceneName)});
                scenes[sceneName] = scene;
            }
        }

        JSON ToJSON()
        {
            JSON project = JSON("{}");
            project.Set("name", name);
            project.Set("path", path);
            project.Set("width", width);
            project.Set("height", height);
            project.Set("title", title);
            project.Set("defaultResourceFile", defaultResourceFile);
            project.Set("sceneNames", scenes.Keys());
            return project;
        }

        String GetProjectFilePath()
        {
            return JoinPath({path, name});
        }

        void AddNewScene(const String &sceneName)
        {
            Ref<SceneInfo> scene = CreateRef<SceneInfo>();
            scene->sceneName = sceneName;
            scene->filePath = JoinPath({path, "scenes", format("{}.json", sceneName)});
            scenes[sceneName] = scene;

            if (!IsExist(JoinPath({path, "scenes"})))
            {
                CreateFolder(JoinPath({path, "scenes"}));
            }

            if (!IsExist(scene->filePath))
            {
                OpenFile(scene->filePath);
                Write(R"({
                        "resources": [],
                        "entities": [] 
                    })");
                CloseFile();
            }
        }

        List<ResourceInfo> GetDefaultResourcesInfo()
        {
            List<ResourceInfo> resources;

            auto defaultConfigFile =
                JoinPath({path, defaultResourceFile});

            if (IsExist(defaultConfigFile))
            {
                JSON config(ReadFile(defaultConfigFile));

                auto resourcesInfo = ExtractInfoFromJSON(config);

                for (auto &resource : resourcesInfo)
                {
                    resources.push_back(resource);
                }
            }

            return resources;
        }

        void SaveDefaultResources(List<ResourceInfo> resources)
        {
            List<JSON> jsons =
                resources
                    .Map<JSON>([](const ResourceInfo &info, ...) -> JSON
                               { return info.ToJSON(); });

            auto defaultConfigFile =
                JoinPath({path, defaultResourceFile});

            OpenFile(defaultConfigFile);
            Write(JSON::FromList(jsons).ToString());
            CloseFile();
        }

        b8 ContainScene(Ref<SceneInfo> scene)
        {
            return scenes.Contains(scene->sceneName);
        }
    };

    struct EditorConfig
    {
        String lastProjectPath;
        String lastProjectFile;
        f32 editorFontSize = 2.0f;

        EditorConfig() = default;

        void From(const JSON &config)
        {
            lastProjectPath = config.Get<String>("lastProjectPath", ".");
            lastProjectFile = config.Get<String>("lastProjectFile", "");
            editorFontSize = config.Get<f32>("editorFontSize", 2.0f);
        }

        JSON ToJSON()
        {
            JSON config = JSON("{}");
            config.Set("lastProjectPath", lastProjectPath);
            config.Set("lastProjectFile", lastProjectFile);
            config.Set("editorFontSize", editorFontSize);
            return config;
        }

        String GetConfigFilePath()
        {
            return JoinPath({CurrentDirectory(), "config.json"});
        }
    };
} // namespace ntt
