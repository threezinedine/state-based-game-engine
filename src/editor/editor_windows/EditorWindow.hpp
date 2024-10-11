#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt
{
    /**
     * Basic form of one window inside the game engine editor. All the windows
     *      should at least implement these methods for easy management.
     */
    class EditorWindow
    {
    public:
        virtual ~EditorWindow() = default;

        /**
         * Window startup configuration which will be called once at the starting
         *      of the editor
         */
        virtual void Init() = 0;

        /**
         * Function which will both drawing and updating the logic of the window.
         * This function will be called each frame of the editor
         */
        virtual void Update() = 0;

        /**
         * Release all the resources of the editor. Called once at the end of the
         *      editor (when the editor is closed).
         */
        virtual void Shutdown() = 0;
    };

} // namespace ntt
