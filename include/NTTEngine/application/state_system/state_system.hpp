#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/ecs/data_com.hpp>
#include <NTTEngine/application/script_system/native_system.hpp>
#include <NTTEngine/application/script_system/native_system.hpp>

#define KEEP_STATE ""

namespace ntt
{
    using namespace memory;
    using namespace ecs;
    using namespace script;

    /**
     * The base of the state class.
     */
    class State : public Scriptable
    {
    public:
        State(Dictionary<String, Ref<State>> children = {}, String defaultState = "");
        virtual ~State();

        /**
         * This method will be called when the state is entered (the previous state is exited).
         * With hierarchical state machine, the default children states will be entered first.
         */
        void OnEnter();

        /**
         * This method will be called when the state is exited (the next state is entered).
         */
        void OnExit();

        /**
         * This method will be called every frame if that state is active.
         *
         * @return The next state name, if the state name is
         *      not exist or is KEEP_STATE, the current state will be kept.
         */
        String OnUpdate(f32 delta);

        /**
         * When adding a new child State, if there no child state, then the state will be
         *      the default state. If there's already child state with the same name, then
         *      the new state will replace the old one.
         */
        void AddChild(const String &name, Ref<State> state);

        void SetEntity(entity_id_t id) override;

    protected:
        virtual void OnUpdateImpl(f32 delta) {}
        virtual String OnNavigateImpl() { return KEEP_STATE; }

    private:
        class Impl;
        Scope<Impl> m_impl;
    };

    struct StateComponent : public ComponentBase
    {
        Ref<State> fsm = nullptr;
        // b8 isStart = FALSE;

        StateComponent(Ref<State> fsm)
            : fsm(fsm)
        {
        }

        StateComponent(Dictionary<String, Ref<State>> children, String defaultState)
            : fsm(CreateRef<State>(children, defaultState))
        {
        }
    };

    class StateSystem : public System
    {
    public:
        StateSystem();
        ~StateSystem();

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
