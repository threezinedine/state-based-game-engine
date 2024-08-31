#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/structures/position.hpp>
#include <NTTEngine/structures/size.hpp>

namespace ntt::renderer
{
    /**
     * Graphic Interface for drawing everything the engine pushes into it
     * It no need to worry about whether the object should be displayed or not
     *      or it is an animated object or not, it just receives the data
     *      and draws it on the screen.
     */

    // When changing this type, does not need to change any code in the engine
    using texture_id_t = u32;

    /**
     * Initialize the graphic interface if it is not initialized yet
     *      every other functionalities will be disabled.
     */
    void RendererInit();

    /**
     * The needed information about the texture
     */
    struct Grid
    {
        u8 numRows; ///< The number of rows in the grid of the texture
        u8 numCols; ///< The number of columns in the grid of the texture

        Grid() : numRows(1), numCols(1) {}
        Grid(u8 numRows, u8 numCols) : numRows(numRows), numCols(numCols) {}
    };

    /**
     * Load the image with the given path and return the
     *      texture ID for that image. If the image is not
     *      found, then the default texture will be returned
     *      and the warning will be printed.
     *
     * The texture will be automatically unloaded (if user does not)
     *      when the program is finished (but the warning will be raised).
     *
     * @param path: The path to the image file
     *      if this path is already loaded, then the texture ID
     *      of the image will be returned (the id of  previous loaded image)
     *
     * @param grid: (Optional) The grid of the texture which are how many
     *      rows and columns in the texture, if the grid is not provided
     *      then the default grid will be used (only 1 row and 1 column).
     */
    texture_id_t LoadTexture(const String &path, const Grid &grid = Grid());

    /**
     * The context which the renderer used for drawing
     *      the objects on the screen.
     */
    struct RectContext
    {
        Position position; ///< The position of the object
        Size size;         ///< The size of the object

        RectContext() = default;
    };

    /**
     * Draw the texture, if the Size in the context is default
     *      then the size of the texture will be used.
     *
     * @param texture_id: The ID of the texture which is loaded
     *      if the texture ID is not found, then nothing will
     *      happen and the warning will be printed.
     *
     * @param context: (Optional) The context which the renderer used for drawing
     */
    void DrawTexture(texture_id_t texture_id, const RectContext &context = RectContext());

    /**
     * Unload the texture with the given texture ID, if
     *      the texture ID is not found, then nothing will
     *      happen and the warning will be printed.
     *
     * @param texture_id: The ID of the texture which is loaded
     */
    void UnloadTexture(texture_id_t texture_id);

    /**
     * Unload all the textures which are loaded by the renderer
     *      and make the warning for each unloaded texture.
     */
    void RendererShutdown();
} // namespace ntt::renderer
