#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/renderer/Texture.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/renderer/RenderSystem.hpp>
#include <NTTEngine/renderer/Sprite.hpp>
#include <NTTEngine/platforms/application.hpp>
#include <NTTEngine/core/assertion.hpp>
#include <NTTEngine/application/layer_system/layer_system.hpp>
#include <NTTEngine/ecs/ecs.hpp>

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
        m_impl = CreateScope<Impl>();
    }

    RenderSystem::~RenderSystem()
    {
    }

    void RenderSystem::InitSystemImpl()
    {
    }

    void RenderSystem::InitEntityImpl(entity_id_t id)
    {
        auto geo = ECS_GET_COMPONENT(id, Geometry);
        auto texture = ECS_GET_COMPONENT(id, Texture);

        auto size = ValidateSize(texture->id,
                                 {{geo->x, geo->y},
                                  {geo->width, geo->height},
                                  geo->rotatation});

        geo->width = size.width;
        geo->height = size.height;
    }

    void RenderSystem::UpdateImpl(f32 delta, entity_id_t id)
    {
        auto geo = ECS_GET_COMPONENT(id, Geometry);
        auto texture = ECS_GET_COMPONENT(id, Texture);

        auto windowSize = GetWindowSize();

        RectContext context;
        context.position = {geo->x, geo->y};
        context.size = {geo->width, geo->height};
        context.rotate = geo->rotatation;

        Grid cell;
        cell.row = texture->rowIndex;
        cell.col = texture->colIndex;

        // check if the texture is in the window or not
        // if not, then don't draw it
        if (geo->x + geo->width / 2 < 0 || geo->x - geo->width / 2 > windowSize.width || geo->y + geo->height / 2 < 0 || geo->y - geo->height / 2 > windowSize.height)
        {
            return;
        }

        auto drawContext = DrawContext(texture->priority);

        if (ECSGetEntity(id)->active)
        {
            drawContext.priority += 10;
        }

        DrawTexture(texture->id, context, cell, drawContext);
    }

    b8 RenderSystem::ShouldUpdate(entity_id_t id)
    {
        auto entities = DrawedEntities();
        return entities.Contains(id);
    }

    void RenderSystem::ShutdownEntityImpl(entity_id_t id)
    {
    }

    void RenderSystem::ShutdownSystemImpl()
    {
    }
} // namespace ntt::renderer
