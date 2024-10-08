#include "GraphicAPI.hpp"
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/structures/list.hpp>

namespace ntt::renderer
{
    class FakeGraphicAPI : public GraphicAPI
    {
    public:
        FakeGraphicAPI();
        ~FakeGraphicAPI();

        Texture2D LoadTexture(const String &path) override;
        void UnloadTexture(Texture2D) override;
        b8 IsLoadedSuccess(Texture2D) override;

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

        u8 m_drawTextCalled;
        u8 m_drawRectangleCalled;
        u8 m_drawRectangleProCalled;
        u8 m_drawTextureCalled;
        List<String> m_drawTexts;
        Texture2D m_expectedTexture;

        static FakeGraphicAPI *s_instance;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt::renderer