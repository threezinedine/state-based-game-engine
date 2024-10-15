#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/renderer/Grid.hpp>

namespace ntt
{
    /**
     * Starting the clipboard system which resets all the internal
     *      data of the clipboard system and allocate the needed memory
     *      for the clipboard system.
     */
    void EditorClipboard_Init();

    /**
     * Store the grid information (row and column) to the clipboard system
     * This method should be used in the image window when the user clicks
     *      into the cell of an image.
     *
     * @param rowIndex The index of the row in the grid
     * @param colIndex The index of the column in the grid
     */
    void EditorClipboard_ChooseGrid(u8 rowIndex, u8 colIndex);

    /**
     * Get the grid information (row and column) from the clipboard system
     *      if the grid information is not available, it will return the
     *      default grid information (0x0 grid).
     *
     * @return The grid information (row and column) if available, otherwise
     *      the default grid information (0x0 grid).
     */
    Grid EditorClipboard_GetGrid();

    /**
     * Clear/remove all internal states of the clipboard system and
     *      free the memory/needed resources.
     */
    void
    EditorClipboard_Shutdown();
} // namespace ntt
