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

    void TextRenderSystem::InitSystem()
    {
        PROFILE_FUNCTION();
    }

    void TextRenderSystem::InitEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
    }

    void TextRenderSystem::Update(f32 delta, entity_id_t id)
    {
        PROFILE_FUNCTION();

        auto geo = ECS_GET_COMPONENT(id, Geometry);
        auto text = ECS_GET_COMPONENT(id, Text);

        DrawContext drawContext;
        drawContext.priority = text->priority;
        drawContext.fontSize = text->fontSize;
        drawContext.color = text->color;

        DrawText(text->text, {geo->x, geo->y}, drawContext);
    }

    void TextRenderSystem::ShutdownEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
    }

    void TextRenderSystem::ShutdownSystem()
    {
        PROFILE_FUNCTION();
    }
}
