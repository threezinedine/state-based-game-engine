#pragma once
#include "GraphicAPI.hpp"
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    class RaylibGraphicAPI : public GraphicAPI
    {
    public:
        RaylibGraphicAPI();
        ~RaylibGraphicAPI();

        Texture2D LoadTexture(const String &path) override;
        void UnloadTexture(Texture2D texture) override;
        b8 IsLoadedSuccess(Texture2D texture) override;

        void DrawText(
            const String &text,
            f32 x,
            f32 y,
            i32 fontSize,
            const RGBAColor &color) override;

        u32 GetTextWidth(const String &text, i32 fontSize) override;

        void DrawRectangle(f32 x, f32 y, f32 width, f32 height, const RGBAColor &color) override;
        void DrawRectanglePro(
            f32 x,
            f32 y,
            f32 width,
            f32 height,
            f32 rotation,
            const RGBAColor &color) override;

        void DrawTexture(Texture2D texture,
                         f32 fx,
                         f32 fy,
                         f32 fw,
                         f32 fh,
                         f32 tx,
                         f32 ty,
                         f32 tw,
                         f32 th,
                         f32 rotate) override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
