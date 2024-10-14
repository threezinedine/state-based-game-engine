#include <NTTEngine/editor/types.hpp>

namespace ntt
{

    void ProjectInfo::From(const JSON &project)
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

    JSON ProjectInfo::ToJSON()
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

    String ProjectInfo::GetProjectFilePath()
    {
        return JoinPath({path, name});
    }

    void ProjectInfo::AddNewScene(const String &sceneName)
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

    void ProjectInfo::ReloadDefaultResourcesInfo()
    {
        defaultResources.clear();

        auto defaultConfigFile =
            JoinPath({path, defaultResourceFile});

        if (IsExist(defaultConfigFile))
        {
            JSON config(ReadFile(defaultConfigFile));

            auto resourcesInfo = ExtractInfoFromJSON(config);

            for (auto &resource : resourcesInfo)
            {
                defaultResources.push_back(resource);
            }
        }
    }

    void ProjectInfo::SaveDefaultResources()
    {
        List<JSON> jsons =
            defaultResources
                .Map<JSON>([](const ResourceInfo &info, ...) -> JSON
                           { return info.ToJSON(); });

        auto defaultConfigFile =
            JoinPath({path, defaultResourceFile});

        OpenFile(defaultConfigFile);
        Write(JSON::FromList(jsons).ToString());
        CloseFile();
    }

    b8 ProjectInfo::ContainScene(Ref<SceneInfo> scene)
    {
        return scenes.Contains(scene->sceneName);
    }

} // namespace ntt