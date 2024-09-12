#include <NTTEngine/application/state_system/state_system.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/ecs/ecs.hpp>

namespace ntt
{
    using namespace log;
    using namespace ecs;

    namespace
    {
        b8 s_initialized = FALSE;
    } // namespace

#define THIS(var) this->m_impl->var

    class State::Impl
    {
    public:
        Dictionary<String, Ref<State>> m_children;
        String m_defaultState;
        String m_currentState;
    };

    State::State(Dictionary<String, Ref<State>> children, String defaultState)
        : m_impl(CreateScope<Impl>())
    {
        THIS(m_children) = children;
        THIS(m_defaultState) = defaultState;
        THIS(m_currentState) = defaultState;
    }

    State::~State()
    {
    }

    void State::SetEntity(entity_id_t id)
    {
        entity_id = id;

        if (THIS(m_children).empty())
        {
            return;
        }

        for (auto &child : THIS(m_children))
        {
            child.second->SetEntity(id);
        }
    }

    void State::AddChild(const String &name, Ref<State> state)
    {
        if (THIS(m_children).empty())
        {
            THIS(m_defaultState) = name;
            THIS(m_currentState) = name;
        }

        THIS(m_children[name]) = state;
    }

    void State::OnEnter()
    {
        OnEnterImpl();

        if (THIS(m_children).empty())
        {
            return;
        }

        THIS(m_currentState) = THIS(m_defaultState);
        THIS(m_children[THIS(m_defaultState)]->OnEnter());
    }

    void State::OnExit()
    {
        OnExitImpl();

        if (THIS(m_children).empty())
        {
            return;
        }

        THIS(m_children[THIS(m_currentState)]->OnExit());
    }

    String State::OnUpdate(f32 delta)
    {
        auto navigatTo = OnNavigateImpl();
        if (THIS(m_children).empty())
        {
            if (navigatTo == KEEP_STATE)
            {
                OnUpdateImpl(delta);
            }
            return navigatTo;
        }

        auto newState = THIS(m_children)[THIS(m_currentState)]->OnUpdate(delta);

        if (THIS(m_currentState) != newState && newState != KEEP_STATE)
        {
            THIS(m_children[THIS(m_currentState)]->OnExit());
            THIS(m_children[newState]->OnEnter());
            THIS(m_currentState) = newState;
            THIS(m_children[newState]->OnUpdate(delta));
        }

        if (navigatTo == KEEP_STATE)
        {
            OnUpdateImpl(delta);
        }

        return navigatTo;
    }

    void StateInit()
    {
        if (s_initialized)
        {
            NTT_ENGINE_WARN("State system already initialized.");
            return;
        }

        s_initialized = TRUE;
    }

    void StateInitFunc(entity_id_t id)
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

    void StateUpdate(f32 delta, entity_id_t id)
    {
        auto machine = ECS_GET_COMPONENT(id, StateComponent);
        if (machine == nullptr)
        {
            NTT_ENGINE_WARN("State component not found.");
            return;
        }

        machine->fsm->OnUpdate(delta);
    }

    void StateShutdownFunc(entity_id_t id)
    {
        auto machine = ECS_GET_COMPONENT(id, StateComponent);
        if (machine == nullptr)
        {
            NTT_ENGINE_WARN("State component not found.");
            return;
        }

        machine->fsm->OnExit();
    }

    void StateShutdown()
    {
        if (!s_initialized)
        {
            NTT_ENGINE_WARN("State system already shutdown or not initialized.");
            return;
        }

        s_initialized = FALSE;
    }
} // namespace ntt
