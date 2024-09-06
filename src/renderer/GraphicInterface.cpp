#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/assertion.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <cstring>
#include <NTTEngine/dev/store.hpp>

#include "GraphicInterface_platforms.hpp"

namespace ntt::renderer
{
    using namespace log;
    using namespace memory;
    using namespace dev::store;

#define TEXTURE_MAX 1000

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
        texture_id_t texture_id;
        f32 fromX;
        f32 fromY;
        f32 fromWidth;
        f32 fromHeight;
        f32 toX;
        f32 toY;
        f32 toWidth;
        f32 toHeight;
        f32 rotate;
    };

    namespace
    {
        b8 s_isInitialized = FALSE;
        Scope<Store<texture_id_t, TextureInfo, String>> s_textureStore;

        List<DrawInfo> s_drawList;
        List<DrawInfo> s_priorityDrawList;
    } // namespace

    void RendererInit()
    {
        if (s_isInitialized)
        {
            return;
        }

        s_textureStore = CreateScope<Store<texture_id_t, TextureInfo, String>>(
            DEFAULT_TEXTURE,
            TEXTURE_MAX,
            [](Ref<TextureInfo> texture)
            { return texture->path; });

        s_isInitialized = TRUE;
        // memset(s_loadedTextures, 0, sizeof(s_loadedTextures));
    }

    texture_id_t LoadTexture(const String &path, const Grid &grid)
    {
        if (!s_isInitialized)
        {
            NTT_ENGINE_ERROR("The renderer is not initialized yet");
            return DEFAULT_TEXTURE;
        }

        if (s_textureStore->ContainsUnique(path))
        {
            NTT_ENGINE_WARN("The texture is already loaded",
                            GetFileName(path, true));
            return DEFAULT_TEXTURE;
        }

        if (IsExist(path) == FALSE)
        {
            NTT_ENGINE_WARN("The texture is not found: {}", GetFileName(path, true));
            return DEFAULT_TEXTURE;
        }

        auto texture = LOAD_TEXTURE(path);

        if (IS_LOADED_SUCCESS(texture) == TRUE)
        {
            NTT_ENGINE_WARN("Loading the texture {} error", GetFileName(path, true));
            return DEFAULT_TEXTURE;
        }

        auto textureInfo = CREATE_REF(TextureInfo, texture, grid, path);

        textureInfo->frameWith = static_cast<f32>(texture.width) / grid.col;
        textureInfo->frameHeight = static_cast<f32>(texture.height) / grid.row;
        return s_textureStore->Add(textureInfo);
    }

    Size DrawTexture(texture_id_t texture_id,
                     const RectContext &context,
                     const Grid &cell,
                     const DrawContext &drawContext)
    {
        if (!s_isInitialized)
        {
            NTT_ENGINE_ERROR("The renderer is not initialized yet");
            return {};
        }

        auto textureInfo = s_textureStore->Get(texture_id);
        if (textureInfo == nullptr)
        {
            return {};
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

        if (drawContext.priority)
        {
            s_priorityDrawList.Add(
                {texture_id,
                 frameWidth * frame.col,
                 frameHeight * frame.row,
                 frameWidth,
                 frameHeight,
                 static_cast<f32>(context.position.x),
                 static_cast<f32>(context.position.y),
                 width,
                 height,
                 static_cast<f32>(context.rotate)});
        }
        else
        {
            s_drawList.Add(
                {texture_id,
                 frameWidth * frame.col,
                 frameHeight * frame.row,
                 frameWidth,
                 frameHeight,
                 static_cast<f32>(context.position.x),
                 static_cast<f32>(context.position.y),
                 width,
                 height,
                 static_cast<f32>(context.rotate)});
        }
        // DRAW_TEXTURE(texture,
        //              frameWidth * frame.col,
        //              frameHeight * frame.row,
        //              frameWidth,
        //              frameHeight,
        //              static_cast<f32>(context.position.x),
        //              static_cast<f32>(context.position.y),
        //              width,
        //              height,
        //              static_cast<f32>(context.rotate));

        return {static_cast<size_t>(width), static_cast<size_t>(height)};
    }

    void GraphicUpdate()
    {
        for (auto i = 0; i < s_drawList.Length(); i++)
        {
            auto drawInfo = s_drawList[i];
            DRAW_TEXTURE(s_textureStore->Get(drawInfo.texture_id)->texture,
                         drawInfo.fromX,
                         drawInfo.fromY,
                         drawInfo.fromWidth,
                         drawInfo.fromHeight,
                         drawInfo.toX,
                         drawInfo.toY,
                         drawInfo.toWidth,
                         drawInfo.toHeight,
                         drawInfo.rotate);
        }

        for (auto i = 0; i < s_priorityDrawList.Length(); i++)
        {
            auto drawInfo = s_priorityDrawList[i];
            DRAW_TEXTURE(s_textureStore->Get(drawInfo.texture_id)->texture,
                         drawInfo.fromX,
                         drawInfo.fromY,
                         drawInfo.fromWidth,
                         drawInfo.fromHeight,
                         drawInfo.toX,
                         drawInfo.toY,
                         drawInfo.toWidth,
                         drawInfo.toHeight,
                         drawInfo.rotate);
        }

        s_drawList.Clear();
        s_priorityDrawList.Clear();
    }

    void UnloadTexture(texture_id_t texture_id)
    {
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

        NTT_ENGINE_DEBUG("Unloading the texture with the ID {} ...", texture_id);
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
        if (!s_isInitialized)
        {
            return;
        }

        ForEachFunc<texture_id_t, TextureInfo> func = [&](Ref<TextureInfo> texture, texture_id_t id)
        {
            NTT_ENGINE_WARN("The texture with the ID {} is not unloaded, unloading ...", id);
            UnloadTexture(id);
        };

        s_textureStore->ForEach(func);

        s_isInitialized = FALSE;

        ASSERT_M(s_textureStore->GetAvailableIds().Length() == 0,
                 "The texture store is not empty");

        s_textureStore.reset();
    }
} // namespace ntt::renderer