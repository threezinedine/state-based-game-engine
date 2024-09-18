#include <NTTEngine/renderer/RenderSystem.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/application/layer_system/layer_system.hpp>
#include <NTTEngine/renderer/Text.hpp>

namespace ntt::renderer
{
    class TextRenderSystem::Impl
    {
    public:
    };

    TextRenderSystem::TextRenderSystem()
        : System()
    {
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();
    }

    TextRenderSystem::~TextRenderSystem()
    {
        PROFILE_FUNCTION();
    }

    void TextRenderSystem::InitSystemImpl()
    {
        PROFILE_FUNCTION();
    }

    void TextRenderSystem::InitEntityImpl(entity_id_t id)
    {
        PROFILE_FUNCTION();
    }

    void TextRenderSystem::UpdateImpl(f32 delta, entity_id_t id)
    {
        PROFILE_FUNCTION();

        auto geo = ECS_GET_COMPONENT(id, Geometry);
        auto text = ECS_GET_COMPONENT(id, Text);

        DrawContext drawContext;
        drawContext.priority = text->priority;
        drawContext.fontSize = text->fontSize;

        DrawText(text->text, {geo->x, geo->y}, drawContext);
    }

    b8 TextRenderSystem::ShouldUpdate(entity_id_t id)
    {
        PROFILE_FUNCTION();
        return DrawnEntities().Contains(id);
    }

    void TextRenderSystem::ShutdownEntityImpl(entity_id_t id)
    {
        PROFILE_FUNCTION();
    }

    void TextRenderSystem::ShutdownSystemImpl()
    {
        PROFILE_FUNCTION();
    }
}
