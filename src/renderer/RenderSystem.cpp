#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/renderer/Texture.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/renderer/RenderSystem.hpp>
#include <NTTEngine/renderer/Sprite.hpp>
#include <NTTEngine/platforms/application.hpp>
#include <NTTEngine/core/assertion.hpp>

namespace ntt::renderer
{
    void RenderInitFunc(entity_id_t id)
    {
        auto geo = ECS_GET_COMPONENT(id, Geometry);
        auto texture = ECS_GET_COMPONENT(id, Texture);

        auto size = ValidateSize(texture->id,
                                 {{geo->x, geo->y}, {geo->width, geo->height}, geo->rotatation});

        geo->width = size.width;
        geo->height = size.height;
    }

    void RenderFunc(f32 delta, entity_id_t id)
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

        auto size = DrawTexture(texture->id, context, cell, DrawContext(texture->priority));
        geo->width = size.width;
        geo->height = size.height;
    }

    void RenderShutdownFunc(entity_id_t id)
    {
    }

    void SpriteRenderFunc(f32 delta, entity_id_t id)
    {
        auto sprite = ECS_GET_COMPONENT(id, Sprite);
        auto texture = ECS_GET_COMPONENT(id, Texture);

        auto colIndex = texture->colIndex;
        auto rowIndex = texture->rowIndex;

        auto currentCell = sprite->cells[sprite->currentCell];

        if (sprite->timer.GetMilliseconds() > sprite->changePerMilis)
        {
            sprite->timer.Reset();
            sprite->currentCell = (sprite->currentCell + 1) % sprite->cells.size();
        }

        if (colIndex != currentCell.first || rowIndex != currentCell.second)
        {
            texture->rowIndex = currentCell.first;
            texture->colIndex = currentCell.second;
        }
    }
} // namespace ntt::renderer
