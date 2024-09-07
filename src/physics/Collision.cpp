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

    namespace
    {
        b8 s_initialized = FALSE;

        Dictionary<entity_id_t, CollisionCallback> s_collisionCallbacks;
    } // namespace

    void CollisionInit()
    {
        if (s_initialized)
        {
            NTT_ENGINE_WARN("Collision system already initialized.");
            return;
        }

        s_initialized = TRUE;
    }

    void CollisionRegister(entity_id_t entity_id, CollisionCallback callback)
    {
        if (!s_initialized)
        {
            NTT_ENGINE_WARN("Collision system not initialized.");
            return;
        }

        s_collisionCallbacks[entity_id] = callback;
    }

    void CollisionFunc(f32 delta, entity_id_t entity_id)
    {
        if (!s_initialized)
        {
            NTT_ENGINE_WARN("Collision system not initialized.");
            return;
        }

        if (s_collisionCallbacks.Contains(entity_id) == FALSE)
        {
            return;
        }

        List<entity_id_t> colliedEntities;
        auto geo = ECS_GET_COMPONENT(entity_id, Geometry);
        auto halfWidth = geo->width / 2;
        auto halfHeight = geo->height / 2;

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

        s_collisionCallbacks[entity_id](colliedEntities);
    }

    void CollisionShutdown()
    {
        if (!s_initialized)
        {
            NTT_ENGINE_WARN("Collision system not initialized.");
            return;
        }

        s_collisionCallbacks.clear();
        s_initialized = FALSE;
    }
} // namespace ntt::physics
