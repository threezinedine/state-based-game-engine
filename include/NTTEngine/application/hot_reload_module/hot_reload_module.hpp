#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <functional>

namespace ntt
{
    /**
     * The hot reload system provide the mechanism to watching and reloading a file from the
     *      file system. Each time the file is changed, the event of file updated will be triggered
     *      and the file module will be reloaded.
     */

    using namespace memory;
    using namespace ecs;
    using script_id_t = u32;
    constexpr script_id_t INVALID_SCRIPT_ID = -1;

    using script_object_id_t = u32;
    constexpr script_object_id_t INVALID_OBJECT_ID = -1;

    /**
     * Specify some simple standards for the module which is loadded into the system, in that
     *      module (.dll or .so) file, they must provide a pair create-destroy method for
     *      a pointer, for example:
     *      ```
     *          extern "C"
     *          {
     *              void* CreateInstance(void* data);   ///< the createFunc now is "CreateInstance"
     *              void DeleteInstance(void*);         ///< pass the pointer from CreateInstance
     *                                                  ///< for deleting it
     *                                                  ///< the deleteFunc now is "DeleteInstance"
     *          }
     *      ```
     * @param factoryName the name of the create function which the module must export
     * @param deleteName the name of the delete function which the module must export
     * @param folder the folder which the module is located (the folder will be fixed throughout
     *      the programming, all script will be placed in this folder). The script in this can be
     *      linked with the module so it can use every interface of this module. This module also
     *      support the recursive folder, all scripts file inside this folder will be compiled and
     *      place at the same level as the <game>.exe file.
     */
    void HotReloadInit(
        const char *createFunc,
        const char *deleteFunc,
        const char *folder);

    /**
     * The script will be loaded into the system, the Create-Delete pair function will be loadded
     *      and stored in the system. The file will be watched for any changes, if the file is
     *      changed, the `NTT_FILE_CHANGED` event will be triggered and the file will be reloaded (
     *      the onLoad function will be called).
     *      Other scripts which use the script_id should be reloadded too.
     * Until the script is loadded successfully, the `NTT_FILE_UPDATED` event will be triggered.
     *
     * @param file the file path of the script, the file can have recursive folder like
     *      `game-script/start.cpp`
     * @param onLoad the function will be called when the file is reloaded.
     */
    script_id_t HotReloadLoad(const char *file, std::function<void()> onLoad);

    /**
     * When unload the script, then all needed resources from that script will be deleted.
     */
    void HotReloadUnload(script_id_t id);

    /**
     * Create new object from the script with the given id script, the data creation will be passed
     *      to the createFunc function of the script.
     *
     * If the script id is not found, then return `INVALID_OBJECT_ID`.
     * Each object created from the script will be separated
     * Use provided macro to automatically cast the object to the correct type.
     *
     * @param id the id of the script
     * @param data the data which will be passed to the create function of the script
     * @return the object id which can be used during the game without being changed
     */
    script_object_id_t HotReloadCreate(script_id_t id, void *data);

    /**
     * Get created object via Id, the object id will not be changed if the script is reloadded.
     *
     * @param id the id of the object
     * @return the object which is created from the script (must be casted for using)
     *      if the object is not found or is invalidated (like the module is unloaded
     *      not reloaded), then return nullptr
     */
    void *HotReloadGetObject(script_object_id_t id);

    /**
     * Unload script module, all objects which is created from the script will be invalidated.
     *
     * @param id the id of the script (if the id is not found, then nothing will be changed)
     */
    void HotReloadUnload(script_id_t id);

    /**
     * Delete, free all needed resources for the hot reload system.
     */
    void HotReloadShutdown();
} // namespace ntt

#define GET_SCRIPT(type, id) std::reinterpret_cast<type *>(HotReloadGetObject(id))

/**
 * This macro must be used in all script files which needed to be loaded into the system.
 */
#define SCRIPT_DEFINE                     \
    extern "C"                            \
    {                                     \
        void *CreateInstance(void *data); \
        void DeleteInstance(void *);      \
    }
