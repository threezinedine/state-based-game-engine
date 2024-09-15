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

    protected:
        void InitSystemImpl() override;
        void InitEntityImpl(entity_id_t id) override;
        void UpdateImpl(f32 delta, entity_id_t id) override;
        void ShutdownEntityImpl(entity_id_t id) override;
        void ShutdownSystemImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
