#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt
{
    enum ToolType
    {
        MOVE,
        SCALE,
        ROTATE,
    };

    void EditorToolInit();

    /**
     * Drawing the tool window with the ToolType options, if the tool is selected
     *      then the NTT_EDITOR_TOOL_TYPE_CHANGED event will be triggered
     */
    void EditorToolUpdate();

    void EditorToolShutdown();
} // namespace ntt
