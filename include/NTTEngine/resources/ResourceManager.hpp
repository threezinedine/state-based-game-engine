#pragma once
#include "resource_common.h"
#include <NTTEngine/core/parser/json.hpp>

namespace ntt
{
    constexpr resource_id_t INVALID_RESOURCE_ID = -1;

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
     * Adding a new resource to the resource manager.
     *
     * @param sceneName The name of the scene which the resource
     *      belongs to. If the scene does not exist, then the
     *      resource will not be added.
     * @param info The information of the resource.
     */
    void RegisterResource(const String &sceneName, const ResourceInfo &info);

    /**
     * Starting the resource manager, now all DEFAULT resources will be loadded,
     *     and the ChangeScene function can be called.
     */
    void ResourceStart();

    /**
     * Retrieve the resource id from the name of that resource.
     * If the resource is not found, the INVALID_RESOURCE_ID will
     *      be returned.
     *
     * @param name The name of the resource.
     */
    resource_id_t GetResourceID(const String &name);

    /**
     * When new scene is loaded, all resources which are not
     *      default resources will be unloaded, and the needed
     *      resource of the new scene will be loaded.
     *
     * Must be called after the ResourceStart function.
     * This function will be delayed until the end of the frame.
     *
     * @param sceneName The name of the new scene. If the scene
     *      does not found, the the warning will be logged and
     *      nothing will be changed.
     */
    void ChangeScene(const String &sceneName);

    /**
     * Actually change the scene, the `ChangeScene` function
     *      will be delayed until the end of the frame.
     */
    void ResourceUpdate(f32 delta);

    /**
     * Shutdown the resource manager.
     */
    void ResourceShutdown();
} // namespace ntt
