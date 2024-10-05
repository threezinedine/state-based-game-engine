#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <functional>

namespace ntt
{
    struct SceneContext
    {
        /**
         * The function which will be called when the scene is opened
         * If the function is not set, nothing will be called
         */
        std::function<void()> createFunc;

        /**
         * The function which will be called when the scene is reloaded
         * If the function is not set, nothing will be called
         */
        std::function<void()> onExit;
    };

    /**
     * The scene system is located inside the GAME_LAYER, each scene has their own
     *      initialization function, no update or shutdown function.
     */

    /**
     * Starting up the scene system, and the sceneNames and the init functions
     *      will be stored in the dictionary.
     *
     * @param scenes: The list of pair the scene name and the init function
     *      - The scene name which is the first key of the dictionary must match with
     *          the scene name of the resources.json file (or no resources will be loaded)
     *      - The first pair of the list will be the default scene
     *      - If the list has no pair, the warning will be logged the game can not run.
     *
     * @param onSceneChanged: The function which will be called when the scene is changed
     *      - If the function is not set, nothing will be called
     */
    void SceneInit(List<std::pair<String, SceneContext>> scenes,
                   std::function<void(const String &)> onSceneChanged = nullptr);

    /**
     * Running the scene with name attribute, all the resources which are related to the scene
     *      will be loaded and initialized, the previous scene and its resources will be deleted.
     *
     * If the scene is not found, the warning will be logged and nothing will be changed.
     * When this function is called successfully, then SCENE_CHANGED event will be triggered with
     *      the data is the name of the scene (so that the name of the scene should less than 15
     *      characters). If the scene name > 15 characters, the warning will be logged and the
     *      event only contains the first 15 characters.
     *
     * @param sceneName: The name of the scene which is stored in the dictionary
     */
    void SceneOpen(const String &sceneName);

    /**
     * Reload the current scene, all entities will be reset and re-initialized as the
     *      start of the scene.
     */
    void SceneReload();

    /**
     * Delete all scenes's information and free the memory
     */
    void SceneShutdown();
} // namespace ntt
