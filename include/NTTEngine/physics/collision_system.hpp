#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>

namespace ntt
{
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
} // namespace ntt
