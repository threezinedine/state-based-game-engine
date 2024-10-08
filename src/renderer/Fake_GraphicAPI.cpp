#include "Fake_GraphicAPI.hpp"

namespace ntt::renderer
{
    class FakeGraphicAPI::Impl
    {
    public:
    };

    FakeGraphicAPI *FakeGraphicAPI::s_instance = nullptr;

    FakeGraphicAPI::FakeGraphicAPI()
        : m_impl(CreateScope<Impl>())
    {

        m_drawTextCalled = 0;
        m_drawRectangleCalled = 0;
        m_drawRectangleProCalled = 0;
        m_drawTextureCalled = 0;

        m_drawTexts = List<String>();
        s_instance = this;
    }

    FakeGraphicAPI::~FakeGraphicAPI()
    {
        s_instance = nullptr;
    }

    Texture2D FakeGraphicAPI::LoadTexture(const String &path)
    {
        return m_expectedTexture;
    }

    void FakeGraphicAPI::UnloadTexture(Texture2D)
    {
    }

    b8 FakeGraphicAPI::IsLoadedSuccess(Texture2D)
    {
        return TRUE;
    }

    void FakeGraphicAPI::DrawText(
        const String &text,
        f32 x,
        f32 y,
        i32 fontSize,
        const RGBAColor &color)
    {
        m_drawTextCalled++;

        m_drawTexts.push_back(text);
    }

    u32 FakeGraphicAPI::GetTextWidth(const String &text, i32 fontSize)
    {
        return 0;
    }

    void FakeGraphicAPI::DrawRectangle(f32 x, f32 y, f32 width, f32 height, const RGBAColor &color)
    {
        m_drawRectangleCalled++;
    }

    void FakeGraphicAPI::DrawRectanglePro(
        f32 x,
        f32 y,
        f32 width,
        f32 height,
        f32 rotation,
        const RGBAColor &color)
    {
        m_drawRectangleProCalled++;
    }

    void FakeGraphicAPI::DrawTexture(Texture2D texture,
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
        m_drawTextureCalled++;
    }
} // namespace ntt::renderer
