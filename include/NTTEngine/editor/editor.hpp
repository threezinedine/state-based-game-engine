#pragma once
#include <NTTEngine/defines.hpp>
#include "editor_log_handler.hpp"
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/string.hpp>

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
    void EditorInit(const String &assetPath);

    /**
     * When this function is called, at the end of the frame, the game
     *      in the viewport window will be started, other features of the game
     *      editor will be turn off.
     */
    void EditorRun();

    /**
     * When this function is called, at the end of the frame, the game
     *      in the viewport window will be stopped, the scene will be
     *      reload and the game can be run again.
     */
    void EditorStop();

    /**
     * Actually apply all the changes in the editor like `EditorRun` or `EditorStop`
     *      and drawing the whole game editor.
     */
    void EditorUpdate(f32 delta);

    /**
     * Called at the start of the drawing frame
     */
    void EditorBeginDraw();

    /**
     * Called at the end of the drawing frame
     */
    void EditorEndDraw();

    /**
     * Shutdown the editor mode in the game engine
     */
    void EditorShutdown();
} // namespace ntt
