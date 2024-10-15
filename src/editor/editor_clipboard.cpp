#include <NTTEngine/editor/editor_clipboard.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    namespace
    {
        Grid s_grid{0, 0};
    } // namespace

    void EditorClipboard_Init()
    {
        PROFILE_FUNCTION();
        s_grid = Grid{0, 0};
    }

    void EditorClipboard_ChooseGrid(u8 rowIndex, u8 colIndex)
    {
        PROFILE_FUNCTION();
        s_grid = Grid{rowIndex, colIndex};
    }

    Grid EditorClipboard_GetGrid()
    {
        PROFILE_FUNCTION();
        return s_grid;
    }

    void EditorClipboard_Shutdown()
    {
        PROFILE_FUNCTION();
        s_grid = Grid{0, 0};
    }
} // namespace ntt
