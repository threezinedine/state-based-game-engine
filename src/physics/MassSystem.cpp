#include <NTTEngine/physics/MassSystem.hpp>
#include <NTTEngine/physics/Mass.hpp>
#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/core/profiling.hpp>

#define TIME_FACTOR (1.0f / 10)

namespace ntt
{
#define THIS(exp) m_impl->exp

    class MassSystem::Impl
    {
    public:
    };

    MassSystem::MassSystem()
        : System()
    {
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();
    }

    MassSystem::~MassSystem()
    {
        PROFILE_FUNCTION();
    }

    void MassSystem::InitSystem()
    {
        PROFILE_FUNCTION();
    }

    void MassSystem::InitEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
    }

    void MassSystem::Update(f32 delta, entity_id_t id)
    {
        PROFILE_FUNCTION();
        auto mass = ECS_GET_COMPONENT(id, Mass);
        auto geo = ECS_GET_COMPONENT(id, Geometry);

        mass->velocity_x += mass->acc_x * delta * TIME_FACTOR;
        mass->velocity_y += mass->acc_y * delta * TIME_FACTOR;

        geo->pos.x += mass->velocity_x * delta * TIME_FACTOR;
        geo->pos.y += mass->velocity_y * delta * TIME_FACTOR;
    }

    void MassSystem::ShutdownEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
    }

    void MassSystem::ShutdownSystem()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt
