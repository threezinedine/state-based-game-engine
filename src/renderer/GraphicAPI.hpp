#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/color.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
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
            const RGBAColor &color = {0, 0, 0, 0}) = 0;
        virtual u32 GetTextWidth(const String &text, i32 fontSize) = 0;

        virtual void DrawRectangle(
            f32 x,
            f32 y,
            f32 width,
            f32 height,
            const RGBAColor &color = {0, 0, 255, 95}) = 0;

        virtual void DrawRectanglePro(
            f32 x,
            f32 y,
            f32 width,
            f32 height,
            f32 rotation,
            const RGBAColor &color = {0, 0, 255, 95}) = 0;

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

        virtual void DrawNoFillRectangle(
            f32 x,
            f32 y,
            f32 width,
            f32 height,
            const RGBAColor &color = {0, 0, 255, 95}) = 0;

        virtual void DrawLine(
            f32 startX,
            f32 startY,
            f32 endX,
            f32 endY,
            const RGBAColor &color = {0, 0, 255, 95},
            u8 lineType = 0) = 0;
    };
} // namespace ntt
