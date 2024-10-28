#pragma once
#include <NTTEngine/defines.hpp>
#include <functional>
#include <NTTEngine/structures/string.hpp>

namespace ntt
{
    /**
     * The hot reload system provide the mechanism to watching and reloading a file from the
     *      file system. Each time the file is changed, the event of file updated will be triggered
     *      and the file module will be reloaded.
     */

    /**
     * Initialize the hot reload system.
     */
    void HotReloadInit();

    /**
     * Reassign the project path for the hot reload system.
     * @param projectPath The path of the project.
     */
    void HotReload_SetProjectPath(const String &projectPath);

    /**
     * Registering the file which will be watched by the hot reload system.
     * In the debug mode, each time the file has changed, the callback will be called.
     * In the release mode, the callback will be called only once at the beginning if
     *      the file is changed with the previous version.
     *
     * @param filePath The path of the file which will be watched. If the file is not exist,
     *      or the file is not a file or the file is watched, the warning will happen.
     * @param callback The callback function which will be called when the file is changed.
     */
    void HotReloadRegister(const String &filePath, std::function<void(const String &)> callback);

    /**
     * Checking whether the file is watched by the hot reload system or not.
     *
     * @param filePath The path of the file to be checked.
     * @return TRUE if the file is watched, FALSE otherwise.
     */
    b8 HotReload_RegisterContains(const String &filePath);

    /**
     * Trigger the event of file updated for all tracked files, this function is used in build
     *      action when the whole controller and state of the game need to be recompiled (the temporary
     *      folder will be deleted.
     */
    void HotReload_ReloadAll();

    /**
     * Delete, free all needed resources for the hot reload system.
     */
    void HotReloadShutdown();
} // namespace ntt