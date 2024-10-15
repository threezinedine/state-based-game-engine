#include <NTTEngine/application/script_system/state.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/application/script_system/script_store.hpp>

namespace ntt
{
    class StateSystem::Impl
    {
    };

    StateSystem::StateSystem()
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
            NTT_ENGINE_WARN("The State component is not found");
            return;
        }

        resource_id_t defaultScriptId = INVALID_SCRIPT_ID;

        for (auto pair : machine->stateScriptIds)
        {
            if (ScriptStoreGetBaseType(pair.second) != typeid(State))
            {
                NTT_ENGINE_WARN("The script is not a State script");
                return;
            }

            if (pair.first == machine->defaultState)
            {
                defaultScriptId = pair.second;
            }
        }

        machine->state = CreateRef<State>();

        auto defaultStateId = ScriptStoreCreate(defaultScriptId, nullptr);
        machine->stateObjIds.push_back(defaultStateId);
        auto defaultState = std::reinterpret_pointer_cast<State>(ScriptStoreGetObject(defaultStateId));
        machine->state->AddChild(machine->defaultState, defaultState);

        for (auto pair : machine->stateScriptIds)
        {
            if (pair.first == machine->defaultState)
            {
                continue;
            }

            auto stateId = ScriptStoreCreate(pair.second, nullptr);
            machine->stateObjIds.push_back(stateId);

            auto stateVoid = ScriptStoreGetObject(stateId);
            if (stateVoid == nullptr)
            {
                NTT_ENGINE_WARN("The State object is not created");
                return;
            }
            auto state = std::reinterpret_pointer_cast<State>(stateVoid);
            machine->state->AddChild(pair.first, state);
        }

        machine->state->SetEntity(id);
        machine->state->OnEnter();
    }

    void StateSystem::Update(f32 delta, entity_id_t id)
    {
        PROFILE_FUNCTION();

        auto machine = ECS_GET_COMPONENT(id, StateComponent);

        if (machine == nullptr)
        {
            NTT_ENGINE_WARN("The State component is not found");
            return;
        }

        if (machine->state == nullptr)
        {
            if (!machine->stateObjIds.empty())
            {
                for (auto stateId : machine->stateObjIds)
                {
                    ScriptStoreDeleteObject(stateId);
                }

                machine->stateObjIds.clear();
            }

            NTT_ENGINE_WARN("The State is not initialized");
            return;
        }
        machine->state->OnUpdate(delta);
    }

    void StateSystem::ShutdownEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();

        auto machine = ECS_GET_COMPONENT(id, StateComponent);

        if (machine == nullptr)
        {
            NTT_ENGINE_WARN("The State component is not found");
            return;
        }

        if (machine->state == nullptr)
        {
            if (!machine->stateObjIds.empty())
            {
                for (auto stateId : machine->stateObjIds)
                {
                    ScriptStoreDeleteObject(stateId);
                }
            }

            NTT_ENGINE_WARN("The State is not initialized");
            return;
        }

        machine->state->OnExit();

        if (!machine->stateObjIds.empty())
        {
            for (auto stateId : machine->stateObjIds)
            {
                ScriptStoreDeleteObject(stateId);
            }
        }

        machine->stateObjIds.clear();
        machine->state = nullptr;
    }

    void StateSystem::ShutdownSystem()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt
