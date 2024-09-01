#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/assertion.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/core/memory.hpp>
#include <cstring>

#ifdef PLATFORM_WINDOWS
#include <raylib.h>

namespace ntt::renderer
{
#define TEXTURE_MAX 1000

    /**
     * All the needed information for rendering the texture
     */
    struct TextureInfo
    {
        Texture2D texture; ///< The texture which is loaded
        Grid grid;         ///< The grid of the texture
        String path;       ///< The path of the texture

        TextureInfo(Texture2D texture, const String &path) : texture(texture), path(path) {}
        TextureInfo(Texture2D texture, const Grid &grid, const String &path)
            : texture(texture), grid(grid), path(path) {}
    };

    namespace
    {
        // If the new texture is loaded then its ID will be
        //      the current texture ID if that texture is not
        //      loaded yet, else the previous texture ID will be returned
        texture_id_t s_currentTextureId = 0;

        // The list of loaded textures
        Scope<TextureInfo> s_loadedTextures[TEXTURE_MAX];

        Dictionary<String, texture_id_t> s_loadedTexturesPath;
    } // namespace

    void RendererInit()
    {
        memset(s_loadedTextures, 0, sizeof(s_loadedTextures));
    }

    texture_id_t LoadTexture(const String &path, const Grid &grid)
    {
        if (s_loadedTexturesPath.Contains(path))
        {
            NTT_ENGINE_WARN("The texture is already loaded",
                            path.RawString().c_str());
            return s_loadedTexturesPath.Get(path);
        }

        auto texture = ::LoadTexture(path.RawString().c_str());

        s_loadedTextures[s_currentTextureId] = CreateScope<TextureInfo>(texture, grid, path);
        s_loadedTexturesPath.Insert(path, s_currentTextureId);
        return s_currentTextureId++;
    }

    void DrawTexture(texture_id_t texture_id, const RectContext &context)
    {
        if (s_loadedTextures[texture_id] == nullptr)
        {
            NTT_ENGINE_WARN("The texture with the ID {} is not loaded yet", texture_id);
            return;
        }

        auto texture = s_loadedTextures[texture_id]->texture;
        f32 textureWidth = static_cast<f32>(s_loadedTextures[texture_id]->texture.width);
        f32 textureHeight = static_cast<f32>(s_loadedTextures[texture_id]->texture.height);
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
            height = (f32)(textureHeight * context.size.width / textureWidth);
        }
        else if (context.size.height != SIZE_DEFAULT)
        {
            width = (f32)(textureWidth * context.size.height / textureHeight);
            height = static_cast<f32>(textureHeight);
        }
        else
        {
            width = static_cast<f32>(textureWidth);
            height = static_cast<f32>(textureHeight);
        }

        NTT_ENGINE_TRACE("Drawing the texture with the ID {} with Rect ({}, {}, {}, {})",
                         texture_id,
                         static_cast<u32>(context.position.x),
                         static_cast<u32>(context.position.y),
                         width,
                         height);

        ::DrawTexturePro(
            texture,
            ::Rectangle{0, 0,
                        textureWidth,
                        textureHeight},
            ::Rectangle{static_cast<f32>(context.position.x),
                        static_cast<f32>(context.position.y),
                        width, height},
            ::Vector2{0, 0},
            0.0f,
            ::WHITE);
    }

    void UnloadTexture(texture_id_t texture_id)
    {
        if (s_loadedTextures[texture_id] == nullptr)
        {
            NTT_ENGINE_WARN("The texture with the ID {} is already unloaded", texture_id);
            return;
        }

        auto texture = s_loadedTextures[texture_id]->texture;

        ::UnloadTexture(texture);
        s_loadedTexturesPath.Remove(s_loadedTextures[texture_id]->path);
        s_loadedTextures[texture_id].reset();

        ASSERT_M(s_loadedTextures[texture_id] == nullptr, "The texture is not unloaded");
    }

    void RendererShutdown()
    {
        for (u32 i = 0; i < TEXTURE_MAX; i++)
        {
            if (s_loadedTextures[i] != nullptr)
            {
                UnloadTexture(i);
                NTT_ENGINE_WARN("The texture with the ID {} is not unloaded", i);
            }
        }
        ASSERT_M(s_loadedTexturesPath.Count() == 0, "The loaded textures path is not empty");
    }
} // namespace ntt::renderer

#endif