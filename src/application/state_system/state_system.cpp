#include <NTTEngine/application/state_system/state_system.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    using namespace log;
    using namespace ecs;

#define THIS(var) this->m_impl->var

    class StateSystem::Impl
    {
    public:
    };

    StateSystem::StateSystem()
        : System()
    {
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();
    }

    StateSystem::~StateSystem()
    {
        PROFILE_FUNCTION();
    }

    void StateSystem::InitSystem()
    {
        PROFILE_FUNCTION();
    }

    void StateSystem::InitEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
        auto machine = ECS_GET_COMPONENT(id, StateComponent);

        if (machine == nullptr)
        {
            NTT_ENGINE_WARN("State component not found.");
            return;
        }

        machine->fsm->SetEntity(id);
        machine->fsm->OnEnter();
    }

    void StateSystem::Update(f32 delta, entity_id_t id)
    {
        PROFILE_FUNCTION();
        auto machine = ECS_GET_COMPONENT(id, StateComponent);
        if (machine == nullptr)
        {
            NTT_ENGINE_WARN("State component not found.");
            return;
        }

        machine->fsm->OnUpdate(delta);
    }

    void StateSystem::ShutdownEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
        auto machine = ECS_GET_COMPONENT(id, StateComponent);
        if (machine == nullptr)
        {
            NTT_ENGINE_WARN("State component not found.");
            return;
        }

        machine->fsm->OnExit();
    }

    void StateSystem::ShutdownSystem()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt
