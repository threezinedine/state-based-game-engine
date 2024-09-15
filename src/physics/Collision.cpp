#include <NTTEngine/physics/Collision.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/renderer/Geometry.hpp>
#include <cmath>

namespace ntt::physics
{
    using namespace log;
    using namespace ecs;
    using namespace renderer;

#define THIS(exp) m_impl->exp

    class CollisionSystem::Impl
    {
    public:
    };

    CollisionSystem::CollisionSystem()
        : System()
    {
        m_impl = CreateScope<Impl>();
    }

    CollisionSystem::~CollisionSystem()
    {
    }

    void CollisionSystem::InitSystemImpl()
    {
    }

    void CollisionSystem::InitEntityImpl(entity_id_t entity_id)
    {
    }

    void CollisionSystem::UpdateImpl(f32 delta, entity_id_t entity_id)
    {
        auto collisionComponent = ECS_GET_COMPONENT(entity_id, Collision);

        if (collisionComponent->callback == nullptr)
        {
            return;
        }

        List<entity_id_t> colliedEntities;
        auto geo = ECS_GET_COMPONENT(entity_id, Geometry);
        auto halfWidth = geo->width / 2;
        auto halfHeight = geo->height / 2;

        if (geo->active == FALSE)
        {
            return;
        }

        auto entities = ECSGetEntitiesWithSystem(COLLISION_NAME);

        List<entity_id_t> others;

        for (auto entity : entities)
        {
            if (entity == entity_id)
            {
                continue;
            }

            others.push_back(entity);
        }

        for (auto other : others)
        {
            auto otherGeo = ECS_GET_COMPONENT(other, Geometry);

            auto halfOtherWidth = otherGeo->width / 2;
            auto halfOtherHeight = otherGeo->height / 2;

            b8 overlapX = std::abs(geo->x - otherGeo->x) < halfWidth + halfOtherWidth;
            b8 overlapY = std::abs(geo->y - otherGeo->y) < halfHeight + halfOtherHeight;

            if (overlapX && overlapY)
            {
                colliedEntities.push_back(other);
            }
        }

        if (colliedEntities.size() == 0)
        {
            return;
        }

        collisionComponent->callback(colliedEntities);
    }

    void CollisionSystem::ShutdownEntityImpl(entity_id_t id)
    {
    }

    void CollisionSystem::ShutdownSystemImpl()
    {
    }
} // namespace ntt::physics
