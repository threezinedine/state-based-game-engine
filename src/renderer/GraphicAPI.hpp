#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/color.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt::renderer
{
    using namespace memory;

    struct Texture2D
    {
        Ref<void> texture;
        f32 width;
        f32 height;

        Texture2D() = default;
        Texture2D(Ref<void> texture, f32 width, f32 height)
            : texture(texture), width(width), height(height) {}
        Texture2D(const Texture2D &texture)
            : texture(texture.texture), width(texture.width), height(texture.height) {}
    };

    class GraphicAPI
    {
    public:
        virtual Texture2D LoadTexture(const String &path) = 0;
        virtual void UnloadTexture(Texture2D) = 0;
        virtual b8 IsLoadedSuccess(Texture2D) = 0;

        virtual void DrawText(
            const String &text,
            f32 x,
            f32 y,
            i32 fontSize,
            Color color) = 0;
        virtual u32 GetTextWidth(const String &text, i32 fontSize) = 0;

        virtual void DrawRectangle(f32 x, f32 y, f32 width, f32 height) = 0;
        virtual void DrawRectanglePro(
            f32 x,
            f32 y,
            f32 width,
            f32 height,
            f32 rotation) = 0;

        virtual void DrawTexture(Texture2D texture,
                                 f32 fx,
                                 f32 fy,
                                 f32 fw,
                                 f32 fh,
                                 f32 tx,
                                 f32 ty,
                                 f32 tw,
                                 f32 th,
                                 f32 rotate) = 0;
    };
} // namespace ntt::renderer