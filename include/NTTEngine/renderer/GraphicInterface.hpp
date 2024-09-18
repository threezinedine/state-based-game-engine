#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/structures/position.hpp>
#include <NTTEngine/structures/size.hpp>
#include <NTTEngine/resources/resource_common.h>
#include <NTTEngine/structures/stack.hpp>

namespace ntt::renderer
{
    /**
     * Graphic Interface for drawing everything the engine pushes into it
     * It no need to worry about whether the object should be displayed or not
     *      or it is an animated object or not, it just receives the data
     *      and draws it on the screen.
     */

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
        u8 row; ///< The number of rows in the grid of the texture
        u8 col; ///< The number of columns in the grid of the texture

        Grid() : row(1), col(1) {}
        Grid(u8 row, u8 col) : row(row), col(col) {}
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
     *      if the path is not found, then the default texture will be returned.
     *
     * @param grid: (Optional) The grid of the texture which are how many
     *      rows and columns in the texture, if the grid is not provided
     *      then the default grid will be used (only 1 row and 1 column).
     */
    resource_id_t LoadTexture(const String &path, const Grid &grid = Grid{1, 1});

    /**
     * The context which the renderer used for drawing
     *      the objects on the screen.
     */
    struct RectContext
    {
        Position position; ///< The position of the object
        Size size;         ///< The size of the object
        f32 rotate;

        RectContext() = default;
    };

    // Additional information for the drawing context
    struct DrawContext
    {
        u8 priority; ///< The priority of the object, the higher priority will be drawn
                     ///< on the top of the lower priority objects (use for both texture
                     ///< and text drawing)

        u32 fontSize; ///< The size of the text which should be displayed (use for drawing
                      ///< text only)

        DrawContext() : priority(0), fontSize(10) {}
    };

    /**
     * Use for validating the size of the texture with some texture which
     *      use the default size (auto ratio size). The function will be
     *      call at the begging of the creation of the object.
     */
    Size ValidateSize(resource_id_t textureId, const RectContext &context);

    /**
     * Draw the texture, if the Size in the context is default
     *      then the size of the texture will be used. The object
     *      will not be drawn immediately, it will be drawn
     *      at the end of the frame (when the GraphicUpdate is called).
     *
     * @param texture_id: The ID of the texture which is loaded
     *      if the texture ID is not found, then nothing will
     *      happen and the warning will be printed.
     *
     * @param context: (Optional) The context which the renderer used for drawing
     * @param cell: (Optional) The cell of the texture which should be drawn
     *      if the cell is not provided then the default grid will be used (0, 0).
     *      if the cell is out of the grid, then the warning will be printed and the
     *      maximum cell will be used.
     * @param drawContext: (Optional) The additional information for the drawing context
     */
    void DrawTexture(resource_id_t texture_id,
                     const RectContext &context = RectContext(),
                     const Grid &cell = Grid{0, 0},
                     const DrawContext &drawContext = DrawContext{});

    /**
     * Display the text on the screen with the given size and some additional
     *      information.
     *
     * @param text: The text to be displayed
     * @param context: (Optional) The context which the renderer used for drawing.
     *      Default position is (0, 0)
     * @param drawContext: (Optional) The additional information for the drawing context
     */
    void DrawText(const String &text,
                  const Position &position = Position(),
                  const DrawContext &drawContext = DrawContext{});

    /**
     * Each texture which is hovered by the mouse will be returned
     *      in the list of the texture ID. If the mouse is not hovered
     *      on any texture, then the empty  list will be returned.
     * This result is the result from the previouse Update call.
     *
     * @return The list of the texture ID which is hovered by the mouse
     *      the higher priority texture will be on the top of the list
     */
    const List<resource_id_t> &GetHoveredTexture();

    /**
     * Actually draw the objects on the screen
     */
    void GraphicUpdate();

    /**
     * Unload the texture with the given texture ID, if
     *      the texture ID is not found, then nothing will
     *      happen and the warning will be printed.
     *
     * @param texture_id: The ID of the texture which is loaded
     */
    void UnloadTexture(resource_id_t texture_id);

    /**
     * Unload all the textures which are loaded by the renderer
     *      and make the warning for each unloaded texture.
     */
    void RendererShutdown();
} // namespace ntt::renderer
