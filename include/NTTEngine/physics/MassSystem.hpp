#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>

using namespace ntt;
using namespace ntt::ecs;

namespace ntt::physics
{
    class MassSystem : public System
    {
    public:
        MassSystem();
        ~MassSystem();

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
