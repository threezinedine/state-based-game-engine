#include <NTTEngine/physics/MassSystem.hpp>
#include <NTTEngine/physics/Mass.hpp>
#include <NTTEngine/renderer/Geometry.hpp>

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::renderer;

namespace ntt::physics
{
#define THIS(exp) m_impl->exp

    class MassSystem::Impl
    {
    public:
    };

    MassSystem::MassSystem()
        : System()
    {
        m_impl = CreateScope<Impl>();
    }

    MassSystem::~MassSystem()
    {
    }

    void MassSystem::InitSystemImpl()
    {
    }

    void MassSystem::InitEntityImpl(entity_id_t id)
    {
    }

    void MassSystem::UpdateImpl(f32 delta, entity_id_t id)
    {
        auto mass = ECS_GET_COMPONENT(id, Mass);
        auto geo = ECS_GET_COMPONENT(id, Geometry);

        mass->velocity_x += mass->acc_x * delta;
        mass->velocity_y += mass->acc_y * delta;

        geo->x += mass->velocity_x * delta;
        geo->y += mass->velocity_y * delta;
    }

    void MassSystem::ShutdownEntityImpl(entity_id_t id)
    {
    }

    void MassSystem::ShutdownSystemImpl()
    {
    }
} // namespace ntt::physics
