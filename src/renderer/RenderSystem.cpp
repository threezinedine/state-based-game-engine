#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/renderer/TextureComponent.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/renderer/RenderSystem.hpp>
#include <NTTEngine/renderer/Sprite.hpp>
#include <NTTEngine/platforms/application.hpp>
#include <NTTEngine/core/assertion.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/renderer/Text.hpp>

namespace ntt::renderer
{
#define THIS(exp) (m_impl->exp)
    using namespace ecs;

    class RenderSystem::Impl
    {
    public:
    };

    RenderSystem::RenderSystem()
        : System()
    {
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();
    }

    RenderSystem::~RenderSystem()
    {
        PROFILE_FUNCTION();
    }

    void RenderSystem::InitSystem()
    {
        PROFILE_FUNCTION();
    }

    void RenderSystem::InitEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
        auto geo = ECS_GET_COMPONENT(id, Geometry);
        auto texture = ECS_GET_COMPONENT(id, TextureComponent);

        if (texture == nullptr)
        {
            return;
        }

        auto size = ValidateSize(texture->id,
                                 {{geo->x, geo->y},
                                  {geo->width, geo->height},
                                  geo->rotation});

        geo->width = size.width;
        geo->height = size.height;
    }

    void RenderSystem::Update(f32 delta, entity_id_t id)
    {
        PROFILE_FUNCTION();
        auto geo = ECS_GET_COMPONENT(id, Geometry);
        auto texture = ECS_GET_COMPONENT(id, TextureComponent);
        auto text = ECS_GET_COMPONENT(id, Text);

        RectContext context;
        Grid cell;
        auto drawContext = DrawContext();

        auto windowSize = GetWindowSize();

        // check if the texture is in the window or not
        // if not, then don't draw it
        if (geo->x + geo->width / 2 < 0 ||
            geo->x - geo->width / 2 > windowSize.width ||
            geo->y + geo->height / 2 < 0 ||
            geo->y - geo->height / 2 > windowSize.height)
        {
            return;
        }

        if (texture != nullptr)
        {
            context.position = {geo->x, geo->y};
            context.size = {geo->width, geo->height};
            context.rotate = geo->rotation;

            cell.row = texture->rowIndex;
            cell.col = texture->colIndex;

            drawContext.entity_id = id;
            drawContext.priority = texture->priority;
            drawContext.tooltip = texture->tooltip;
            DrawTexture(texture->id, context, cell, drawContext);
        }
        else if (text != nullptr)
        {
            drawContext.priority = text->priority;
            drawContext.fontSize = text->fontSize;
            drawContext.color = text->color;

            DrawText(text->text, {geo->x, geo->y}, drawContext);
        }
    }

    void RenderSystem::ShutdownEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
    }

    void RenderSystem::ShutdownSystem()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt::renderer
