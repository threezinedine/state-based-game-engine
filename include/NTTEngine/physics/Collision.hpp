#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>

#define COLLISION_NAME "Collision"

namespace ntt::physics
{
    using namespace ecs;
    using CollisionCallback = std::function<void(List<entity_id_t>)>;

    /**
     * Collision component with no data (at least for now).
     */
    struct Collision : public ComponentBase
    {
        CollisionCallback callback = nullptr;

        Collision() = default;
    };

    class CollisionSystem : public System
    {
    public:
        CollisionSystem();
        ~CollisionSystem();

        void InitSystem() override;
        void InitEntity(entity_id_t id) override;
        void Update(f32 delta, entity_id_t id) override;
        void ShutdownEntity(entity_id_t id) override;
        void ShutdownSystem() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt::physics
