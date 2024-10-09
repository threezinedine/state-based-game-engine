#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/renderer/TextureComponent.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/renderer/RenderSystem.hpp>
#include <NTTEngine/renderer/Sprite.hpp>
#include <NTTEngine/platforms/application.hpp>
#include <NTTEngine/core/assertion.hpp>
#include <NTTEngine/core/profiling.hpp>

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
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();
    }

    SpriteRenderSystem::~SpriteRenderSystem()
    {
        PROFILE_FUNCTION();
    }

    void SpriteRenderSystem::InitSystem()
    {
        PROFILE_FUNCTION();
    }

    void SpriteRenderSystem::InitEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
    }

    void SpriteRenderSystem::Update(f32 delta, entity_id_t id)
    {
        PROFILE_FUNCTION();
        auto sprite = ECS_GET_COMPONENT(id, Sprite);
        auto texture = ECS_GET_COMPONENT(id, TextureComponent);

        auto rowIndex = texture->currentCell.row;
        auto colIndex = texture->currentCell.col;

        auto currentCell = sprite->cells[sprite->currentCell];

        if (sprite->timer.GetMilliseconds() > sprite->changePerMilis)
        {
            sprite->timer.Reset();
            sprite->currentCell = (sprite->currentCell + 1) % sprite->cells.size();
        }

        if (rowIndex != currentCell.first || rowIndex != currentCell.second)
        {
            texture->currentCell.row = currentCell.first;
            texture->currentCell.col = currentCell.second;
        }
    }

    void SpriteRenderSystem::ShutdownEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
    }

    void SpriteRenderSystem::ShutdownSystem()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt::renderer
