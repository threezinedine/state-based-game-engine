#pragma once
#include "resource_common.h"
#include <NTTEngine/core/parser/json.hpp>

namespace ntt
{
    /**
     * Initialize the resource manager. No scene is active
     *      when the resource manager is initialized. Need to
     *      Change the scene manually.
     */
    void ResourceInit();

    /**
     * Configure the resource manager from the configuration
     *      file content (in JSON format).
     */
    void ResourceLoadConfig(const JSON &config);

    /**
     * Retrieve the resource id from the name of that resource.
     *
     * @param name The name of the resource.
     */
    resource_id_t GetResourceID(const String &name);

    /**
     * When new scene is loaded, all resources which are not
     *      default resources will be unloaded, and the needed
     *      resource of the new scene will be loaded.
     *
     * @param sceneName The name of the new scene. If the scene
     *      does not found, the the warning will be logged and
     *      nothing will be changed.
     */
    void ChangeScene(const String &sceneName);

    /**
     * Shutdown the resource manager.
     */
    void ResourceShutdown();
} // namespace ntt