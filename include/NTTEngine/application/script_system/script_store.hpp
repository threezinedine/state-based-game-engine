#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <functional>
#include <NTTEngine/resources/Resource.hpp>

/**
 * The mechanism for loading dynamically the script for file to
 *      the game
 */
namespace ntt::script
{
    using namespace memory;
    using namespace ecs;

    using resource_id_t = u32;
    constexpr resource_id_t INVALID_SCRIPT_ID = -1;

    using script_object_id_t = u32;
    constexpr script_object_id_t INVALID_OBJECT_ID = -1;

    using CreateFuncType = void *(*)(void *);
    using DeleteFuncType = void (*)(void *);
    using GetBaseTypeFunc = std::type_index (*)();

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
     *              std::type_index GetBaseType();      ///< the base type of the module like Script,
     *                                                  ///< Component, State
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
    void ScriptStoreInit(
        const char *createFunc,
        const char *deleteFunc,
        const char *getBaseTypeFunc);

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
    resource_id_t ScriptStoreLoad(const char *file, std::function<void()> onLoad);

    /**
     * Load script by static code rather than compiling the file
     *
     * @param key the key for the script, it is used for identifying the script
     * @param createFunc the create function of the script
     * @param deleteFunc the delete function of the script
     */
    resource_id_t ScriptStoreLoad(
        const char *key,
        CreateFuncType createFunc,
        DeleteFuncType deleteFunc,
        GetBaseTypeFunc getBaseTypeFunc);

    /**
     * Retrieve the base type of the script (now how 2 options: Script and State)
     *
     * @param id the id of the script, if the id is not found
     *      or INVALID_SCRIPT_ID, then return `std::type_index(typeid(void))`
     */
    std::type_index ScriptStoreGetBaseType(resource_id_t id);

    /**
     * Use for reloading the script, firstly, the module will be free, then the callback
     *      will be called, after that, the module will be reloaded.
     *
     * All other object which is created from the script will be automatically reloaded
     *     (the script_object_id_t is not changed).
     */
    void ScriptStoreReload(resource_id_t id, std::function<void()> callback);

    /**
     * When unload the script, then all needed resources from that script will be deleted.
     */
    void ScriptStoreUnload(resource_id_t id);

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
    script_object_id_t ScriptStoreCreate(resource_id_t id, void *data);

    /**
     * Get created object via Id, the object id will not be changed if the script is reloadded.
     *
     * @param id the id of the object
     * @return the object which is created from the script (must be casted for using)
     *      if the object is not found or is invalidated (like the module is unloaded
     *      not reloaded), then return nullptr
     */
    void *ScriptStoreGetObject(script_object_id_t id);

    /**
     * Free the object only, the module will not be unloaded.
     *
     * @param id the id of the object
     */
    void ScriptStoreDeleteObject(script_object_id_t id);

    /**
     * Unload script module, all objects which is created from the script will be invalidated.
     *
     * @param id the id of the script (if the id is not found, then nothing will be changed)
     */
    void ScriptStoreUnload(resource_id_t id);

    /**
     * Delete, free all needed resources for the hot reload system.
     */
    void ScriptStoreShutdown();
} // namespace ntt::script

#define GET_SCRIPT(type, id) reinterpret_cast<type *>(ScriptStoreGetObject(id))

/**
 * This macro must be used in all script files which needed to be loaded into the system.
 */
#define SCRIPT_DEFINE(type, base)                            \
    extern "C"                                               \
    {                                                        \
        void *CreateInstance(void *data)                     \
        {                                                    \
            return reinterpret_cast<void *>(new type(data)); \
        }                                                    \
        void DeleteInstance(void *obj)                       \
        {                                                    \
            delete reinterpret_cast<type *>(obj);            \
        }                                                    \
        std::type_index GetBaseType()                        \
        {                                                    \
            return std::type_index(typeid(base));            \
        }                                                    \
    }

#include "native_script.hpp"