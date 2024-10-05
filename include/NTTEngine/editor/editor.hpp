#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt
{
    /**
     * Initialize the editor mode in the game engine
     *
     * @param use: If the editor mode is used or not.
     *      if TRUE, the game will be stand at the start of the
     *      scene. If FALSE, the game will be run as normal and
     *      other command will be ignored.
     */
    void EditorInit(b8 use = TRUE);

    /**
     * Starting the game until the EditorStop is called.
     * If the game is already running, then nothing will happen.
     */
    void EditorRun();

    /**
     * Stop the editor mode in the game engine. If the game
     *      is already stand, then nothing will happen.
     */
    void EditorStop();

    /**
     * Where the `EditorRun` and `EditorStop` are called
     *      actually in the game engine.
     */
    void EditorUpdate(f32 delta);

    /**
     * Shutdown the editor mode in the game engine
     */
    void EditorShutdown();
} // namespace ntt
