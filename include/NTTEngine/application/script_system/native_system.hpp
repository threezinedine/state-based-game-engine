#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

namespace ntt::script
{
    using namespace ecs;
    using namespace event;

    struct NativeScriptComponent;

    class Script
    {
    public:
        void OnCreate();
        void OnDestroy();
        void OnUpdate(f32 deltaTime);

        inline void SetEntity(entity_id_t id) { entity_id = id; }
        inline b8 IsInitialized() const { return entity_id != INVALID_ENTITY_ID; }
        inline entity_id_t GetEntity() const { return entity_id; }

    protected:
        virtual void OnCreateImpl() {}
        virtual void OnDestroyImpl() {}
        virtual void OnUpdateImpl(f32 deltaTime) {}

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

        void Subscribe(event_code_t eventCode, EventCallback callback);
        void Delete();

    private:
        entity_id_t entity_id = INVALID_ENTITY_ID;
        List<event_id_t> events;
        b8 m_deleted = FALSE;
    };

    struct NativeScriptComponent : public ComponentBase
    {
        Ref<Script> ins = nullptr;

        NativeScriptComponent(Ref<Script> instance)
            : ins(instance) {}
    };

    /**
     * Starting the Script system
     */
    void ScriptInit();

    /**
     * Handling all Script which is passed to the system
     */
    void ScriptUpdate(f32 deltaTime, entity_id_t entity_id);

    /**
     * Shutdown the Script system
     */
    void ScriptShutdown();
} // namespace ntt::script
