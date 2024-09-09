#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>

namespace ntt::script
{
    using namespace ecs;

    struct NativeScriptComponent;

    class Script
    {
    public:
        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(f32 deltaTime) {}

        inline void SetEntity(entity_id_t id) { entity_id = id; }
        inline b8 IsInitialized() const { return entity_id != INVALID_ENTITY_ID; }

    protected:
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

    private:
        entity_id_t entity_id = INVALID_ENTITY_ID;
        friend struct NativeScriptComponent;
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
