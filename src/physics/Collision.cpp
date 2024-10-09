#include <NTTEngine/physics/Collision.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/core/profiling.hpp>
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
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();
    }

    CollisionSystem::~CollisionSystem()
    {
        PROFILE_FUNCTION();
    }

    void CollisionSystem::InitSystem()
    {
        PROFILE_FUNCTION();
    }

    void CollisionSystem::InitEntity(entity_id_t entity_id)
    {
        PROFILE_FUNCTION();
    }

    void CollisionSystem::Update(f32 delta, entity_id_t entity_id)
    {
        PROFILE_FUNCTION();
        auto collisionComponent = ECS_GET_COMPONENT(entity_id, Collision);

        if (collisionComponent->callback == nullptr)
        {
            return;
        }

        List<entity_id_t> colliedEntities;
        auto geo = ECS_GET_COMPONENT(entity_id, Geometry);
        auto halfWidth = geo->size.width / 2;
        auto halfHeight = geo->size.height / 2;

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

            auto halfOtherWidth = otherGeo->size.width / 2;
            auto halfOtherHeight = otherGeo->size.height / 2;

            b8 overlapX = std::abs(geo->pos.x - otherGeo->pos.x) < halfWidth + halfOtherWidth;
            b8 overlapY = std::abs(geo->pos.y - otherGeo->pos.y) < halfHeight + halfOtherHeight;

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

    void CollisionSystem::ShutdownEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
    }

    void CollisionSystem::ShutdownSystem()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt::physics
