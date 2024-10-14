#include <NTTEngine/editor/editor_clipboard.hpp>

namespace ntt
{
    namespace
    {
        Grid s_grid{0, 0};
    } // namespace

    void EditorClipboard_Init()
    {
        s_grid = Grid{0, 0};
    }

    void EditorClipboard_ChooseGrid(u8 rowIndex, u8 colIndex)
    {
        s_grid = Grid{rowIndex, colIndex};
    }

    Grid EditorClipboard_GetGrid()
    {
        return s_grid;
    }

    void EditorClipboard_Shutdown()
    {
        s_grid = Grid{0, 0};
    }
} // namespace ntt
