#include "types.hpp"

namespace ntt
{
    void EditorConfig::From(const JSON &config)
    {
        lastProjectPath = config.Get<String>("lastProjectPath", ".");
        lastProjectFile = config.Get<String>("lastProjectFile", "");
        editorFontSize = config.Get<f32>("editorFontSize", 2.0f);
    }

    JSON EditorConfig::ToJSON() const
    {
        JSON config = JSON("{}");
        config.Set("lastProjectPath", lastProjectPath);
        config.Set("lastProjectFile", lastProjectFile);
        config.Set("editorFontSize", editorFontSize);
        return config;
    }

    String EditorConfig::GetConfigFilePath()
    {
        return JoinPath({CurrentDirectory(), "config.json"});
    }
} // namespace ntt
