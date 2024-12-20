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
#include <NTTEngine/renderer/line.hpp>
#include <NTTEngine/resources/ResourceManager.hpp>

namespace ntt
{
#define THIS(exp) (m_impl->exp)

    class RenderSystem::Impl
    {
    public:
        b8 editor;
    };

    RenderSystem::RenderSystem(b8 editor)
        : System()
    {
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();
        m_impl->editor = editor;
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

        if (texture->GetTextureID() == INVALID_RESOURCE_ID)
        {
            geo->size.width = 100;
            geo->size.height = 100;
        }

        texture->textureGrid = GetTextureGrid(texture->GetTextureID());

        auto size = ValidateSize(texture->GetTextureID(),
                                 {geo->pos,
                                  geo->size,
                                  geo->rotation});

        geo->size.width = size.first.width;
        geo->size.height = size.first.height;

        geo->originalSize.width = size.second.width;
        geo->originalSize.height = size.second.height;
    }

    void RenderSystem::Update(f32 delta, entity_id_t id)
    {
        PROFILE_FUNCTION();
        auto geo = ECS_GET_COMPONENT(id, Geometry);
        auto texture = ECS_GET_COMPONENT(id, TextureComponent);
        auto text = ECS_GET_COMPONENT(id, Text);
        auto line = ECS_GET_COMPONENT(id, Line);

        RectContext context;
        Grid cell;
        auto drawContext = DrawContext();

        auto windowSize = GetWindowSize();

        if (geo == nullptr)
        {
            return;
        }

        if (!m_impl->editor)
        {
            // check if the texture is in the window or not
            // if not, then don't draw it
            if (geo->pos.x + geo->size.width / 2 < 0 ||
                geo->pos.x - geo->size.width / 2 > windowSize.width ||
                geo->pos.y + geo->size.height / 2 < 0 ||
                geo->pos.y - geo->size.height / 2 > windowSize.height)
            {
                return;
            }
        }

        drawContext.priority = geo->priority;
        drawContext.entity_id = id;

        if (texture != nullptr)
        {
            context.position = geo->pos;
            context.size = geo->size;
            context.rotate = geo->rotation;

            cell.row = texture->currentCell.row;
            cell.col = texture->currentCell.col;

            drawContext.tooltip = texture->tooltip;

            DrawTexture(texture->GetTextureID(), context, cell, drawContext);
        }
        else if (text != nullptr)
        {
            drawContext.fontSize = text->fontSize;
            drawContext.color = geo->color;

            DrawText(text->text, {geo->pos.x, geo->pos.y}, drawContext);
        }
        else if (line != nullptr)
        {
            drawContext.color = line->color;
            drawContext.lineType = line->lineType;
            drawContext.priority = geo->priority;

            DrawLine(line->start, line->end, drawContext);
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
} // namespace ntt
