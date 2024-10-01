#include <NTTEngine/application/hot_reload_module/hot_reload_module.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/platforms/path.hpp>
// #include "filewatch.hpp"
#include "watch_dog.hpp"

namespace ntt
{
#define FUNC_NAME_BUFFER_SIZE 256
#define PATH_BUFFER_SIZE 256
    using namespace log;

    namespace
    {
        char s_createFunc[FUNC_NAME_BUFFER_SIZE];
        char s_deleteFunc[FUNC_NAME_BUFFER_SIZE];
        String s_folder;

        Dictionary<String, String> s_scriptContents;

        b8 CheckScriptUpdated(const String &file)
        {
            auto filePath = JoinPath({s_folder, file}, FALSE);

            auto newContent = ReadFile(filePath);

            if (s_scriptContents.Contains(file) && s_scriptContents[file] == newContent)
            {
                return FALSE;
            }

            s_scriptContents[file] = newContent;
            return TRUE;
        }
    } // namespace

    void HotReloadInit(
        const char *createFunc,
        const char *deleteFunc,
        const char *folder)
    {
        memcpy(s_createFunc, createFunc, strlen(createFunc) + 1);
        memcpy(s_deleteFunc, deleteFunc, strlen(deleteFunc) + 1);
        s_folder = RelativePath(folder);

        s_scriptContents.clear();
        NTT_ENGINE_DEBUG("Hot reload module is initialized with folder {}.",
                         s_folder);

        wd::watch(s_folder.RawString(), [](const fs::path &path)
                  { NTT_ENGINE_DEBUG("The file {} is changed.", path); });

        // filewatch::FileWatch<std::string> watch(
        //     // StringToWString(s_folder.RawString()),
        //     s_folder.RawString(),
        //     [](const std::string &path, const filewatch::Event change_type)
        //     {
        //         NTT_ENGINE_DEBUG("The file {} is changed.", path);
        //         switch (change_type)
        //         {
        //         case filewatch::Event::added:
        //             break;
        //         case filewatch::Event::removed:
        //             break;
        //         case filewatch::Event::modified:
        //             NTT_ENGINE_DEBUG("The file is modified.");
        //             break;
        //         case filewatch::Event::renamed_old:
        //             break;
        //         // case filewatch::ChangeType::renamed_new:
        //         //     std::cout << "The file was renamed and this is the new name." << '\n';
        //         //     break;
        //     }; });
    }

    script_id_t HotReloadLoad(const char *file, std::function<void()> onLoad)
    {
        return INVALID_SCRIPT_ID;
    }

    script_object_id_t HotReloadCreateObject(script_id_t id)
    {
        return INVALID_OBJECT_ID;
    }

    void *HotReloadGetObject(script_object_id_t id)
    {
        return nullptr;
    }

    void HotReloadUnload(script_id_t id)
    {
    }

    void HotReloadShutdown()
    {
    }
} // namespace ntt
