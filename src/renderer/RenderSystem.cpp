#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/renderer/Texture.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/renderer/RenderSystem.hpp>
#include <NTTEngine/platforms/application.hpp>

namespace ntt::renderer
{
    void RenderFunc(f32 delta, entity_id_t id, List<entity_id_t> others)
    {
        auto geometry = ECS_GET_COMPONENT(id, Geometry);
        auto texture = ECS_GET_COMPONENT(id, Texture);

        auto windowSize = GetWindowSize();

        RectContext context;
        context.position = {geometry->x, geometry->y};
        context.size = {geometry->width, geometry->height};
        context.rotate = geometry->rotatation;

        Grid cell;
        cell.row = texture->rowIndex;
        cell.col = texture->colIndex;

        // check if the texture is in the window or not
        // if not, then don't draw it
        if (geometry->x < 0 || geometry->x > windowSize.width || geometry->y < 0 || geometry->y > windowSize.height)
        {
            return;
        }

        auto size = DrawTexture(texture->id, context, cell);
        geometry->width = size.width;
        geometry->height = size.height;
    }
} // namespace ntt::renderer
