#pragma once
#include "resource_common.h"
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    constexpr resource_id_t INVALID_RESOURCE_ID = -1;

    /**
     * Initialize the resource manager. No scene is active
     *      when the resource manager is initialized. Need to
     *      Change the scene manually.
     *
     * @param editorMode The mode of the resource manager.
     *      TRUE then the script file will be watched and reloaded
     *      FALSE then the script file will not be watched and reloaded
     */
    void ResourceInit(b8 editorMode);

    /**
     * Parsing the JSON content of the resource file to the list of
     *      resource information with some basic information like
     *      type, name, path, and some additional information.
     *
     * @param json The JSON content of the resource file. The JSON
     *      must be a list of resources with template:
     *      [
     *          {
     *              "name": "resource_name",
     *              "type": 0, // 0 is IMAGE
     *              "path": "path/to/resource",
     *              "additionalInfo": {
     *                  "grid":
     *                  {
     *                      "rol": 3,
     *                      "col": 2
     *                  }
     *              },
     *          },
     *          {
     *              "name": "resource_name",
     *              "type": 1, // 1 is AUDIO
     *              "path": "path/to/resource",
     *              "additionalInfo": {
     *              },
     *          }
     *      ]
     *
     * @return The list of resource information.
     */
    List<ResourceInfo> ExtractInfoFromJSON(const JSON &config);

    /**
     * Load all the resources from the list of resource information.
     * If a resource is already loadded (name of it exists) the that
     *      resource will be ignored.
     *
     * @param infos The list of resource information.
     */
    void ResourceLoad(List<ResourceInfo> infos);

    /**
     * Assign the project path
     *
     * @param projectPath The path of the project.
     *      if the path is not exist, the warning will happen.
     *      and the project path will not be changed.
     */
    void Resource_SetProjectPath(const String &projectPath);

    /**
     * Unload all the resources from the list of resource information.
     * If a resource is not loadded (name of it does not exist) the that
     *      resource will be ignored.
     */
    void ResourceUnload(List<ResourceInfo> infos);

    resource_id_t GetResourceID(const String &name);
    String GetResourceName(resource_id_t id);

    /**
     * Shutdown the resource manager.
     */
    void ResourceShutdown();
} // namespace ntt
