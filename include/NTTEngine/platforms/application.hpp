#pragma once
#include <NTTEngine/defines.hpp>
#include <functional>
#include <NTTEngine/structures/size.hpp>
#include <NTTEngine/core/parser/json.hpp>

namespace ntt
{
    /**
     * User can have some defined phrases for the application
     *      it can be provided to the engine via this struct
     *      but this struct is hidden from the user
     *
     */
    struct Phrases
    {
        std::function<void()> Begin;
        std::function<void(f32)> MainLoop;
        std::function<void()> Close;
    };

    /**
     * Making an window of application with certain width, height and title
     *
     * @param screenWidth: the width of the window
     * @param screenHeight: the height of the window
     * @param title: the title of the window
     * @param phrases: the phrases of the application which is defined by the user
     */
    void ApplicationInit(u16 screenWidth,
                         u16 screenHeight,
                         const char *title,
                         const Phrases &phrases,
                         b8 editor = FALSE);

    /**
     * Storing the JSON configuration data for the
     *      whole application should be called right
     *      after setting the source dir in the main
     *
     * @param path: the path to the configuration file if the file is not found
     *     or not json file then the default configuration will be loaded "{}"
     */
    void LoadConfiguration(const String &path);

    /**
     * Get the configuration data of the application
     *      if not loaded then it will return an empty JSON object
     */
    JSON &GetConfiguration();

    /**
     * Main game loop
     *
     * @param running: a boolean reference to check if the game is running or not
     */
    void ApplicationUpdate(b8 &running);

    /**
     * Get the current size of the window
     *
     * @return the size of the window
     */
    Size &GetWindowSize();

    /**
     * Destroying all things inside the game engine, the window will be closed
     *      and all the resources will be freed
     */
    void ApplicationShutdown();
} // namespace ntt
