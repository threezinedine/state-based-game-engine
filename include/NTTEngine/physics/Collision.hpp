#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>

#define COLLISION_NAME "Collision"

namespace ntt::physics
{
    using namespace ecs;

    /**
     * Collision component with no data (at least for now).
     */
    struct Collision : public ComponentBase
    {
        Collision() = default;
    };

    /**
     * Startup the collision system.
     */
    void CollisionInit();

    using CollisionCallback = std::function<void(List<entity_id_t>)>;

    /**
     * Register which callbacks should be called when a collision occurs.
     */
    void CollisionRegister(entity_id_t, CollisionCallback callback);

    /**
     * The system function which is used for regsitering with the ECS.
     */
    void CollisionFunc(f32 delta, entity_id_t entity_id);

    /**
     * Delete the collision system.
     */
    void CollisionShutdown();
} // namespace ntt::physics
