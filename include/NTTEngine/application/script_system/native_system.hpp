#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/ecs/data_com.hpp>

namespace ntt::script
{
    using namespace ecs;
    using namespace event;

    struct NativeScriptComponent;

    class Scriptable
    {
    public:
        virtual ~Scriptable() = default;

        virtual void OnEnter() = 0;
        virtual void OnExit() = 0;

        virtual void SetEntity(entity_id_t id) { entity_id = id; }
        entity_id_t GetEntity() const { return entity_id; }
        b8 IsInitialized() const { return entity_id != INVALID_ENTITY_ID; }

    protected:
        virtual void OnEnterImpl() {}
        virtual void OnExitImpl() {}

        template <typename T>
        Ref<T> GetComponent()
        {
            return ECS_GET_COMPONENT(entity_id, T);
        }

        template <typename T>
        void SetComponentState(b8 state = FALSE)
        {
            ECSSetComponentActive(entity_id, typeid(T), state);
        }

        JSON &GetData()
        {
            auto data = GetComponent<DataComponent>();
            return data->data;
        }

        void Subscribe(event_code_t eventCode, EventCallback callback)
        {
            auto id = RegisterEvent(eventCode, callback);
            events.push_back(id);
        }

        void Delete()
        {
            if (m_deleted)
            {
                return;
            }
            m_deleted = TRUE;

            for (auto id : events)
            {
                UnregisterEvent(id);
            }
            OnExitImpl();
            ECSDeleteEntity(entity_id);
        }

        entity_id_t entity_id = INVALID_ENTITY_ID;
        b8 m_deleted = FALSE;

    private:
        List<event_id_t> events;
    };

    class Script : public Scriptable
    {
    public:
        virtual void OnEnter();
        virtual void OnExit();
        void OnUpdate(f32 deltaTime);

    protected:
        virtual void OnUpdateImpl(f32 deltaTime) {}
    };

    struct NativeScriptComponent : public ComponentBase
    {
        Ref<Script> ins = nullptr;

        NativeScriptComponent(Ref<Script> instance)
            : ins(instance) {}
    };

    class ScriptSystem : public System
    {
    public:
        ScriptSystem();
        ~ScriptSystem();
        // const String GetName() const override { return "Script System"; }

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
} // namespace ntt::script
