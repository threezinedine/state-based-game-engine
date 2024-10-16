#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/ecs/data_com.hpp>
#include <NTTEngine/application/script_system/scriptable.hpp>
#include <NTTEngine/resources/resource_common.h>
#include <NTTEngine/application/script_system/script_store.hpp>

#define KEEP_STATE ""

namespace ntt
{
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
        void OnUpdate(f32 delta);

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

} // namespace ntt
