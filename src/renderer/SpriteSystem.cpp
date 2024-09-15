#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/renderer/Texture.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/renderer/RenderSystem.hpp>
#include <NTTEngine/renderer/Sprite.hpp>
#include <NTTEngine/platforms/application.hpp>
#include <NTTEngine/core/assertion.hpp>

namespace ntt::renderer
{
#define THIS(exp) (m_impl->exp)
    class SpriteRenderSystem::Impl
    {
    public:
    };

    SpriteRenderSystem::SpriteRenderSystem()
        : System()
    {
        m_impl = CreateScope<Impl>();
    }

    SpriteRenderSystem::~SpriteRenderSystem()
    {
    }

    void SpriteRenderSystem::InitSystemImpl()
    {
    }

    void SpriteRenderSystem::InitEntityImpl(entity_id_t id)
    {
    }

    void SpriteRenderSystem::UpdateImpl(f32 delta, entity_id_t id)
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

    void SpriteRenderSystem::ShutdownEntityImpl(entity_id_t id)
    {
    }

    void SpriteRenderSystem::ShutdownSystemImpl()
    {
    }
} // namespace ntt::renderer
