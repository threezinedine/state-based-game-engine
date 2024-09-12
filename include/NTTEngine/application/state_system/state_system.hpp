#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/ecs/data_com.hpp>
#include <NTTEngine/application/script_system/native_system.hpp>

#define KEEP_STATE ""

namespace ntt
{
    using namespace memory;
    using namespace ecs;

    /**
     * The base of the state class.
     */
    class State
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

        void SetId(entity_id_t id);
        entity_id_t GetId() const { return m_id; }

    protected:
        virtual void OnEnterImpl() {}
        virtual void OnExitImpl() {}

        virtual void OnUpdateImpl(f32 delta) {}
        virtual String OnNavigateImpl() { return KEEP_STATE; }

        template <typename T>
        Ref<T> GetComponent()
        {
            return ECS_GET_COMPONENT(m_id, T);
        }

        template <typename T>
        void SetComponentActive(b8 active = TRUE)
        {
            ECSSetComponentActive(m_id, typeid(T), active);
        }

        JSON &GetData()
        {
            auto data = GetComponent<DataComponent>();
            return data->data;
        }

    private:
        entity_id_t m_id = INVALID_ENTITY_ID;

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

    /**
     * @brief Initialize the state system.
     */
    void StateInit();

    /**
     * @brief Update the state system.
     */
    void StateUpdate(f32 delta, entity_id_t entity_id_t);

    /**
     * @brief Shutdown the state system.
     */
    void StateShutdown();
} // namespace ntt
