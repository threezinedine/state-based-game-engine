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
     * Delete, free all needed resources for the hot reload system.
     */
    void HotReloadShutdown();
} // namespace ntt