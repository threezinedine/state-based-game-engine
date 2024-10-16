#include "Raylib_GraphicAPI.hpp"
#include <raylib.h>

namespace ntt
{
    class RaylibGraphicAPI::Impl
    {
    public:
    };

    RaylibGraphicAPI::RaylibGraphicAPI()
        : m_impl(CreateScope<Impl>())
    {
    }

    RaylibGraphicAPI::~RaylibGraphicAPI()
    {
    }

    Texture2D RaylibGraphicAPI::LoadTexture(const String &path)
    {
        auto texture = CreateRef<::Texture2D>(::LoadTexture(path.RawString().c_str()));
        return Texture2D(
            std::static_pointer_cast<void>(texture),
            static_cast<f32>(texture->width),
            static_cast<f32>(texture->height));
    }

    void RaylibGraphicAPI::UnloadTexture(Texture2D texture)
    {
        auto texture2D = std::static_pointer_cast<::Texture2D>(texture.texture);
        ::UnloadTexture(*texture2D);
    }

    b8 RaylibGraphicAPI::IsLoadedSuccess(Texture2D texture)
    {
        return std::static_pointer_cast<::Texture2D>(texture.texture)->id != 0;
    }

    void RaylibGraphicAPI::DrawText(
        const String &text,
        f32 x,
        f32 y,
        i32 fontSize,
        const RGBAColor &color)
    {
        ::DrawText(text.RawString().c_str(), x, y, fontSize, ::Color{color.r, color.g, color.b, color.a});
    }

    u32 RaylibGraphicAPI::GetTextWidth(const String &text, i32 fontSize)
    {
        return ::MeasureText(text.RawString().c_str(), fontSize);
    }

    void RaylibGraphicAPI::DrawRectangle(f32 x, f32 y, f32 width, f32 height, const RGBAColor &color)
    {
        ::DrawRectangle(x, y, width, height, ::Color{color.r, color.g, color.b, color.a});
    }

    void RaylibGraphicAPI::DrawRectanglePro(
        f32 x,
        f32 y,
        f32 width,
        f32 height,
        f32 rotation,
        const RGBAColor &color)
    {
        ::DrawRectanglePro(
            {x,
             y,
             static_cast<f32>(width),
             static_cast<f32>(height)},
            {width / 2, height / 2},
            rotation,
            ::Color{color.r, color.g, color.b, color.a});
    }

    void RaylibGraphicAPI::DrawTexture(Texture2D texture,
                                       f32 fx,
                                       f32 fy,
                                       f32 fw,
                                       f32 fh,
                                       f32 tx,
                                       f32 ty,
                                       f32 tw,
                                       f32 th,
                                       f32 rotate)
    {
        ::Texture2D *texture2D = std::static_pointer_cast<::Texture2D>(texture.texture).get();
        ::DrawTexturePro(*texture2D,
                         ::Rectangle{fx, fy, fw, fh},
                         ::Rectangle{tx, ty, tw, th},
                         ::Vector2{tw / 2, th / 2},
                         rotate,
                         ::WHITE);
    }

    void RaylibGraphicAPI::DrawNoFillRectangle(
        f32 x, f32 y,
        f32 width,
        f32 height,
        const RGBAColor &color)
    {
        constexpr f32 delta = 1.0f;
        ::DrawRectangleLines(
            x - delta,
            y - delta,
            width + delta * 2,
            height + delta * 2,
            ::Color{color.r, color.g, color.b, color.a});
    }
} // namespace ntt
