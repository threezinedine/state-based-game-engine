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
#include <NTTEngine/resources/ResourceManager.hpp>

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

        if (texture->id == INVALID_RESOURCE_ID)
        {
            geo->size.width = 100;
            geo->size.height = 100;
        }

        auto size = ValidateSize(texture->id,
                                 {geo->pos,
                                  geo->size,
                                  geo->rotation});

        geo->size.width = size.width;
        geo->size.height = size.height;
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

        if (geo == nullptr)
        {
            return;
        }

        // check if the texture is in the window or not
        // if not, then don't draw it
        if (geo->pos.x + geo->size.width / 2 < 0 ||
            geo->pos.x - geo->size.width / 2 > windowSize.width ||
            geo->pos.y + geo->size.height / 2 < 0 ||
            geo->pos.y - geo->size.height / 2 > windowSize.height)
        {
            return;
        }

        drawContext.priority = geo->priority;
        drawContext.entity_id = id;

        if (texture != nullptr)
        {
            context.position = geo->pos;
            context.size = geo->size;
            context.rotate = geo->rotation;

            cell.row = texture->rowIndex;
            cell.col = texture->colIndex;

            drawContext.tooltip = texture->tooltip;

            DrawTexture(texture->id, context, cell, drawContext);
        }
        else if (text != nullptr)
        {
            drawContext.fontSize = text->fontSize;
            drawContext.color = geo->color;

            DrawText(text->text, {geo->pos.x, geo->pos.y}, drawContext);
        }
        else
        {
            context.position = geo->pos;
            context.size = geo->size;
            context.rotate = geo->rotation;

            drawContext.color = geo->color;

            DrawRectangle(context, drawContext);
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
