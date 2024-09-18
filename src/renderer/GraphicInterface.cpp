#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/assertion.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <cstring>
#include <NTTEngine/dev/store.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/platforms/application.hpp>
#include <NTTEngine/ecs/ecs.hpp>

#include "GraphicInterface_platforms.hpp"

namespace ntt::renderer
{
    using namespace log;
    using namespace memory;
    using namespace dev::store;
    using namespace ecs;

#define TEXTURE_MAX 1000
#define TOOL_TIP_FONT_SIZE 10
#define TOOL_TIP_PADDING 5
#define TOOL_TOP_OFFSET_X 20
#define TOOL_TOP_OFFSET_Y 20
#define MAX_PRIORITY 20

    /**
     * All the needed information for rendering the texture
     */
    struct TextureInfo
    {
        TEXTURE_2D texture; ///< The texture which is loaded
        Grid grid;          ///< The grid of the texture
        String path;        ///< The path of the texture
        f32 frameWith;      ///< The width each frame in the texture
        f32 frameHeight;    ///< The height each frame in the texture

        TextureInfo(TEXTURE_2D texture, const String &path) : texture(texture), path(path) {}
        TextureInfo(TEXTURE_2D texture, const Grid &grid, const String &path)
            : texture(texture), grid(grid), path(path) {}
    };

    struct DrawInfo
    {
        entity_id_t entity_id;
        resource_id_t texture_id;
        f32 fromX;
        f32 fromY;
        f32 fromWidth;
        f32 fromHeight;
        f32 toX;
        f32 toY;
        f32 toWidth;
        f32 toHeight;
        f32 rotate;
        String tooltip;
        b8 drawText = FALSE;
        String text;
        u32 fontSize;
    };

    namespace
    {
        b8 s_isInitialized = FALSE;
        Scope<Store<resource_id_t, TextureInfo>> s_textureStore;

        // List<DrawInfo> s_drawList;
        // List<DrawInfo> s_priorityDrawList;

        // Store all the priorities draw with maximum 256 priorities
        // The higher priority will be drawn on the top of the lower priority
        // It should be cleared after each frame
        Scope<List<DrawInfo>> s_drawLists[MAX_PRIORITY];

        // Stack of all texture ID which is hovered by the mouse
        // It also be cleared after each frame
        // The higher priority texture will be on the top of the stack
        // If the same priority, the last hovered texture will be on the top
        List<entity_id_t> s_hoveredTextures;

    } // namespace

    void RendererInit()
    {
        PROFILE_FUNCTION();
        if (s_isInitialized)
        {
            return;
        }

        s_textureStore = CreateScope<Store<resource_id_t, TextureInfo>>(
            RESOURCE_ID_DEFAULT,
            TEXTURE_MAX,
            [](Ref<TextureInfo> texture, Ref<TextureInfo> other) -> b8
            { return texture->path == other->path; });

        memset(s_drawLists, 0, sizeof(s_drawLists));

        s_isInitialized = TRUE;
        // memset(s_loadedTextures, 0, sizeof(s_loadedTextures));
    }

    resource_id_t LoadTexture(const String &path, const Grid &grid)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            NTT_ENGINE_ERROR("The renderer is not initialized yet");
            return RESOURCE_ID_DEFAULT;
        }

        auto existedTexturePaths = s_textureStore->GetByField<String>(
            path,
            [](Ref<TextureInfo> texture) -> String
            { return texture->path; });

        if (existedTexturePaths.size() > 0)
        {
            NTT_ENGINE_WARN("The texture is already loaded",
                            GetFileName(path, true));
            return RESOURCE_ID_DEFAULT;
        }

        if (IsExist(path) == FALSE)
        {
            NTT_ENGINE_WARN("The texture is not found: {}", GetFileName(path, true));
            return RESOURCE_ID_DEFAULT;
        }

        auto texture = LOAD_TEXTURE(path);

        if (IS_LOADED_SUCCESS(texture) == TRUE)
        {
            NTT_ENGINE_WARN("Loading the texture {} error", GetFileName(path, true));
            return RESOURCE_ID_DEFAULT;
        }

        auto textureInfo = CREATE_REF(TextureInfo, texture, grid, path);

        textureInfo->frameWith = static_cast<f32>(texture.width) / grid.col;
        textureInfo->frameHeight = static_cast<f32>(texture.height) / grid.row;
        return s_textureStore->Add(textureInfo);
    }

    Size ValidateSize(resource_id_t texture_id,
                      const RectContext &context)
    {
        PROFILE_FUNCTION();
        auto textureInfo = s_textureStore->Get(texture_id);
        if (textureInfo == nullptr)
        {
            return {};
        }

        auto texture = textureInfo->texture;
        f32 textureWidth = static_cast<f32>(textureInfo->texture.width);
        f32 textureHeight = static_cast<f32>(textureInfo->texture.height);
        f32 frameWidth = textureInfo->frameWith;
        f32 frameHeight = textureInfo->frameHeight;
        f32 width = 0.0f;
        f32 height = 0.0f;

        if (context.size.width != SIZE_DEFAULT && context.size.height != SIZE_DEFAULT)
        {
            width = static_cast<f32>(context.size.width);
            height = static_cast<f32>(context.size.height);
        }
        else if (context.size.width != SIZE_DEFAULT)
        {
            width = static_cast<f32>(context.size.width);
            height = (f32)(frameHeight * context.size.width / frameWidth);
        }
        else if (context.size.height != SIZE_DEFAULT)
        {
            width = (f32)(frameWidth * context.size.height / frameHeight);
            height = static_cast<f32>(frameHeight);
        }
        else
        {
            width = static_cast<f32>(frameWidth);
            height = static_cast<f32>(frameHeight);
        }

        return {static_cast<size_t>(width), static_cast<size_t>(height)};
    }

    void DrawTexture(resource_id_t texture_id,
                     const RectContext &context,
                     const Grid &cell,
                     const DrawContext &drawContext)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            NTT_ENGINE_ERROR("The renderer is not initialized yet");
            return;
        }

        if (!s_textureStore->Contains(texture_id))
        {
            NTT_ENGINE_WARN("The texture with the ID {} is not found", texture_id);
            return;
        }

        auto textureInfo = s_textureStore->Get(texture_id);
        if (textureInfo == nullptr)
        {
            return;
        }

        auto grid = textureInfo->grid;
        Grid frame;
        frame.row = static_cast<u8>(cell.row) >= static_cast<u8>(grid.row)
                        ? static_cast<u8>(grid.row - 1)
                        : static_cast<u8>(cell.row);
        frame.col = static_cast<u8>(cell.col) >= static_cast<u8>(grid.col)
                        ? static_cast<u8>(grid.col - 1)
                        : static_cast<u8>(cell.col);
        auto texture = textureInfo->texture;
        f32 frameWidth = textureInfo->frameWith;
        f32 frameHeight = textureInfo->frameHeight;
        auto actualSize = ValidateSize(texture_id, context);

        if (s_drawLists[drawContext.priority] == nullptr)
        {
            s_drawLists[drawContext.priority] = CreateScope<List<DrawInfo>>();
        }

        DrawInfo info;
        info.entity_id = drawContext.entity_id;
        info.texture_id = texture_id;
        info.fromX = frameWidth * frame.col;
        info.fromY = frameHeight * frame.row;
        info.fromWidth = frameWidth;
        info.fromHeight = frameHeight;
        info.toX = static_cast<f32>(context.position.x);
        info.toY = static_cast<f32>(context.position.y);
        info.toWidth = static_cast<f32>(actualSize.width);
        info.toHeight = static_cast<f32>(actualSize.height);
        info.rotate = static_cast<f32>(context.rotate);
        info.tooltip = drawContext.tooltip;
        info.drawText = FALSE;

        s_drawLists[drawContext.priority]->push_back(info);
    }

    void DrawText(const String &text,
                  const Position &position,
                  const DrawContext &drawContext)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            NTT_ENGINE_ERROR("The renderer is not initialized yet");
            return;
        }

        if (s_drawLists[drawContext.priority] == nullptr)
        {
            s_drawLists[drawContext.priority] = CreateScope<List<DrawInfo>>();
        }

        DrawInfo info;
        info.entity_id = drawContext.entity_id;
        info.drawText = TRUE;
        info.text = text;
        info.toX = static_cast<f32>(position.x);
        info.toY = static_cast<f32>(position.y);
        info.fontSize = drawContext.fontSize;

        s_drawLists[drawContext.priority]->push_back(info);
    }

    void GraphicUpdate()
    {
        PROFILE_FUNCTION();
        s_hoveredTextures.clear();

        auto mouse = input::GetMousePosition();

        auto highestPriority = MAX_PRIORITY - 1;

        for (highestPriority; highestPriority >= 0; highestPriority--)
        {
            if (s_drawLists[highestPriority] != nullptr)
            {
                break;
            }
        }

        for (auto i = 0; i < MAX_PRIORITY; i++)
        {
            if (s_drawLists[i] == nullptr)
            {
                continue;
            }

            for (auto info : *s_drawLists[i])
            {
                if (info.drawText)
                {
                    DRAW_TEXT(info.text, info.toX, info.toY, info.fontSize);
                }
                else
                {
                    DRAW_TEXTURE(s_textureStore->Get(info.texture_id)->texture,
                                 info.fromX,
                                 info.fromY,
                                 info.fromWidth,
                                 info.fromHeight,
                                 info.toX,
                                 info.toY,
                                 info.toWidth,
                                 info.toHeight,
                                 info.rotate);

                    if (info.entity_id == INVALID_ENTITY_ID)
                    {
                        continue;
                    }

                    if (info.toX - info.toWidth / 2 <= mouse.x &&
                        mouse.x <= info.toX + info.toWidth / 2 &&
                        info.toY - info.toHeight / 2 <= mouse.y &&
                        mouse.y <= info.toY + info.toHeight / 2)
                    {
                        s_hoveredTextures.push_back(info.entity_id);

                        if (info.tooltip != "" && i == highestPriority)
                        {
                            auto windowSize = GetWindowSize();

                            auto textWidth = GET_TEXT_WIDTH(info.tooltip, TOOL_TIP_FONT_SIZE);
                            auto textHeight = TOOL_TIP_FONT_SIZE;

                            auto toolTipX = mouse.x + TOOL_TOP_OFFSET_X;

                            if (toolTipX + textWidth > windowSize.width)
                            {
                                toolTipX -= (textWidth + TOOL_TIP_PADDING * 2 + TOOL_TOP_OFFSET_X);
                            }

                            auto toolTipY = mouse.y + TOOL_TOP_OFFSET_Y;

                            if (toolTipY + textHeight > windowSize.height)
                            {
                                toolTipY -= textHeight - TOOL_TIP_PADDING * 2 - TOOL_TOP_OFFSET_Y;
                            }

                            DRAW_RECTANGLE(
                                toolTipX,
                                toolTipY,
                                textWidth + TOOL_TIP_PADDING * 2,
                                textHeight + TOOL_TIP_PADDING * 2);

                            DRAW_TEXT(info.tooltip,
                                      toolTipX + TOOL_TIP_PADDING,
                                      toolTipY + TOOL_TIP_PADDING,
                                      TOOL_TIP_FONT_SIZE);
                        }
                    }
                }
            }

            s_drawLists[i]->clear();
            s_drawLists[i].reset();

            ASSERT(s_drawLists[i] == nullptr);
        }
    }

    const List<entity_id_t> &GetHoveredTexture()
    {
        return s_hoveredTextures;
    }

    void UnloadTexture(resource_id_t texture_id)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            NTT_ENGINE_ERROR("The renderer is not initialized yet");
            return;
        }

        if (s_textureStore->Get(texture_id) == nullptr)
        {
            NTT_ENGINE_WARN("The texture with the ID {} is already unloaded", texture_id);
            return;
        }

        try
        {
            UNLOAD_TEXTURE(s_textureStore->Get(texture_id)->texture);
        }
        catch (const std::exception &e)
        {
            NTT_ENGINE_ERROR("Unloading the texture with the ID {} error: {}",
                             texture_id, e.what());
        }
        s_textureStore->Release(texture_id);
    }

    void RendererShutdown()
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return;
        }

        ForEachFunc<resource_id_t, TextureInfo> func = [&](Ref<TextureInfo> texture, resource_id_t id)
        {
            NTT_ENGINE_WARN("The texture with the ID {} is not unloaded, unloading ...", id);
            UnloadTexture(id);
        };

        s_textureStore->ForEach(func);

        s_isInitialized = FALSE;

        ASSERT_M(s_textureStore->GetAvailableIds().size() == 0,
                 "The texture store is not empty");

        s_textureStore.reset();
    }
} // namespace ntt::renderer