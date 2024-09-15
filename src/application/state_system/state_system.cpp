#include <NTTEngine/application/state_system/state_system.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/ecs/ecs.hpp>

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
        m_impl = CreateScope<Impl>();
    }

    StateSystem::~StateSystem()
    {
    }

    void StateSystem::InitSystemImpl()
    {
    }

    void StateSystem::InitEntityImpl(entity_id_t id)
    {
        auto machine = ECS_GET_COMPONENT(id, StateComponent);

        if (machine == nullptr)
        {
            NTT_ENGINE_WARN("State component not found.");
            return;
        }

        machine->fsm->SetEntity(id);
        machine->fsm->OnEnter();
    }

    void StateSystem::UpdateImpl(f32 delta, entity_id_t id)
    {
        auto machine = ECS_GET_COMPONENT(id, StateComponent);
        if (machine == nullptr)
        {
            NTT_ENGINE_WARN("State component not found.");
            return;
        }

        machine->fsm->OnUpdate(delta);
    }

    void StateSystem::ShutdownEntityImpl(entity_id_t id)
    {
        auto machine = ECS_GET_COMPONENT(id, StateComponent);
        if (machine == nullptr)
        {
            NTT_ENGINE_WARN("State component not found.");
            return;
        }

        machine->fsm->OnExit();
    }

    void StateSystem::ShutdownSystemImpl()
    {
    }
} // namespace ntt
