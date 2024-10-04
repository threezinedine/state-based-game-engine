#include <NTTEngine/application/hot_reload_module/hot_reload_module.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/core/logging.hpp>
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

        void Test()
        {
            // auto watch = CreateScope<filewatch::FileWatch<std::string>>(
            //     JoinPath({s_folder, file}, FALSE).RawString(),
            //     [onLoad](const std::string &path, const filewatch::Event change_type)
            //     {
            //     switch (change_type)
            //     {
            //     case filewatch::Event::removed:
            //         break;
            //     case filewatch::Event::modified:
            //         NTT_ENGINE_DEBUG("The file {} is modified.", path);
            //         onLoad();
            //         break;
            //     case filewatch::Event::renamed_old:
            //         break;
            //     // case filewatch::ChangeType::renamed_new:
            //         //     std::cout << "The file was renamed and this is the new name." << '\n';
            //         //     break;
            //     }; });
        }
    } // namespace

    void HotReloadInit()
    {
        PROFILE_FUNCTION();

        s_registered.clear();
    }

    void HotReloadRegister(const String &filePath, std::function<void(const String &)> callback)
    {
        PROFILE_FUNCTION();

        if (s_registered.Contains(filePath))
        {
            NTT_ENGINE_WARN("The file {} is already registered.", filePath);
            return;
        }

        HotReloadData data;
        data.tempPath = SubtractPath(filePath, GetStoredPath(PathType::NTT_SOURCE));
        data.tempPath = JoinPath({GetStoredPath(PathType::NTT_BINARY),
                                  "temp",
                                  data.tempPath},
                                 FALSE);

        if (!IsExist(data.tempPath))
        {
            NTTCopyFile(filePath, data.tempPath);
            callback(filePath);
        }

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

        std::ifstream file2(filePath.RawString(), std::ios::binary);
        if (!file2)
        {
            NTT_ENGINE_FATAL("The file {} cannot be opened.", filePath);
            return;
        }

        if (data.buffer != std::vector<char>(
                               std::istreambuf_iterator<char>(file2),
                               std::istreambuf_iterator<char>()))
        {
            data.buffer.clear();
            std::copy(
                std::istreambuf_iterator<char>(file2),
                std::istreambuf_iterator<char>(),
                std::back_inserter(data.buffer));

            NTTCopyFile(filePath, data.tempPath);
            callback(filePath);
        }
        file2.close();

        data.onLoad = callback;
        s_registered[filePath] = data;
    }

    void HotReloadShutdown()
    {
        PROFILE_FUNCTION();

        s_registered.clear();
    }
} // namespace ntt
