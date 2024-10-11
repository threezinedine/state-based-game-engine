#pragma once

#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/platforms/path.hpp>

namespace ntt
{
    struct ProjectInfo
    {
        String name;
        String path;
        i32 width = 800;
        i32 height = 600;
        String title;
        String defaultResourceFile;

        ProjectInfo() = default;

        void From(const JSON &project)
        {
            name = project.Get<String>("name");
            path = project.Get<String>("path");
            width = project.Get<i32>("width", 800);
            height = project.Get<i32>("height", 600);
            title = project.Get<String>("title");
            defaultResourceFile = project.Get<String>("defaultResourceFile");
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
            return project;
        }

        String GetProjectFilePath()
        {
            return JoinPath({path, name});
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
