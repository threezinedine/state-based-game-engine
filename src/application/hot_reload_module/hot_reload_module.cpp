#include <NTTEngine/application/hot_reload_module/hot_reload_module.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/platforms/path.hpp>
#include "filewatch.hpp"
#include <iostream>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
#define FUNC_NAME_BUFFER_SIZE 256
#define PATH_BUFFER_SIZE 256
    using namespace log;

    namespace
    {
        struct HotReloadData
        {
            std::function<void(const String &)> onLoad;
            std::vector<char> buffer;
            String tempPath;
        };

        Dictionary<String, HotReloadData> s_registered;

        List<String> s_watchedFiles;
        List<filewatch::FileWatch<std::string> *> s_watchers;
        String s_projectPath;

        void Comparing(const String &filePath)
        {
            HotReloadData &data = s_registered[filePath];

            if (!IsExist(data.tempPath))
            {
                NTTCopyFile(filePath, data.tempPath);
                data.onLoad(filePath);
            }

            if (data.buffer.size() == 0)
            {
                std::ifstream file1(data.tempPath.RawString(), std::ios::binary);
                if (!file1)
                {
                    NTT_ENGINE_FATAL("The file {} cannot be opened.", data.tempPath);
                    return;
                }
                data.buffer = std::vector<char>(
                    std::istreambuf_iterator<char>(file1),
                    std::istreambuf_iterator<char>());
                file1.close();
            }

            std::ifstream file2(filePath.RawString(), std::ios::binary);
            auto newBuffer = std::vector<char>(
                std::istreambuf_iterator<char>(file2),
                std::istreambuf_iterator<char>());
            if (data.buffer != newBuffer)
            {
                data.buffer.clear();
                data.buffer = newBuffer;
                NTTCopyFile(filePath, data.tempPath);
                data.onLoad(filePath);
            }
            file2.close();
        }

        void OnFileChanged(const String &path)
        {
            if (!s_watchedFiles.Contains(path))
            {
                return;
            }

            Comparing(path);
        }

        void Listener(const String &path, const filewatch::Event change_type)
        {
            switch (change_type)
            {
            case filewatch::Event::removed:
                break;
            case filewatch::Event::modified:
                NTT_ENGINE_DEBUG("The file {} has been modified.", path);
                OnFileChanged(path);
                break;
            case filewatch::Event::renamed_old:
                break;
            }
        }
    } // namespace

    void HotReloadInit()
    {
        PROFILE_FUNCTION();

        for (auto watcher : s_watchers)
        {
            delete watcher;
        }

        s_projectPath = CurrentDirectory();

        s_watchers.clear();
        s_registered.clear();
        s_watchedFiles.clear();
    }

    void HotReload_SetProjectPath(const String &projectPath)
    {
        PROFILE_FUNCTION();

        if (!IsExist(projectPath))
        {
            NTT_ENGINE_WARN("The project path {} is not exist.", projectPath);
            return;
        }

        s_projectPath = projectPath;
    }

    void HotReloadRegister(const String &filePath, std::function<void(const String &)> callback)
    {
        PROFILE_FUNCTION();

        if (SubtractPath(filePath, s_projectPath) == filePath)
        {
            NTT_ENGINE_WARN("The file {} is not in the project path.", filePath);
            return;
        }

        if (s_registered.Contains(filePath))
        {
            NTT_ENGINE_WARN("The file {} is already registered.", filePath);
            return;
        }

        HotReloadData data;
        data.tempPath = SubtractPath(filePath, s_projectPath);
        data.tempPath = JoinPath({s_projectPath,
                                  "temp",
                                  data.tempPath});
        data.onLoad = callback;
        s_registered[filePath] = data;

        Comparing(filePath);

        auto folder = GetFileFolder(filePath);

        if (!s_watchedFiles.Contains(filePath))
        {
            s_watchedFiles.push_back(filePath);
            s_watchers.push_back(
                new filewatch::FileWatch<std::string>(
                    folder.RawString(),
                    [filePath](const std::string &path, filewatch::Event change_type)
                    {
                        Listener(filePath, change_type);
                    }));
        }
    }

    b8 HotReload_RegisterContains(const String &filePath)
    {
        PROFILE_FUNCTION();

        return s_registered.Contains(filePath);
    }

    void HotReloadShutdown()
    {
        PROFILE_FUNCTION();

        for (auto watcher : s_watchers)
        {
            delete watcher;
        }

        s_watchers.clear();

        s_registered.clear();
        s_watchedFiles.clear();
    }
} // namespace ntt
