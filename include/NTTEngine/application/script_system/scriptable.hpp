#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/object.hpp>

namespace ntt
{
    class Scriptable : public Object
    {
    public:
        virtual ~Scriptable() = default;

        virtual void OnEnter() = 0;
        virtual void OnExit() = 0;
        virtual void OnUpdate(f32 delta) = 0;

        virtual inline void SetEntity(entity_id_t id) { entity_id = id; }
        inline entity_id_t GetEntity() const { return entity_id; }
        inline b8 IsInitialized() const { return entity_id != INVALID_ENTITY_ID; }

    protected:
        virtual void OnEnterImpl() {}
        virtual void OnExitImpl() {}
        virtual void OnUpdateImpl(f32 delta) {}

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

        JSON &GetData();
        void Subscribe(event_code_t eventCode);
        void Delete();

        virtual void OnEvent(
            event_code_t eventCode,
            void *sender,
            const EventContext &context) {}

        entity_id_t entity_id = INVALID_ENTITY_ID;

    private:
        b8 m_deleted = FALSE;
        List<event_id_t> events;
    };

} // namespace ntt