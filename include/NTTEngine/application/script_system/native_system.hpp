#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/ecs/data_com.hpp>
#include "scriptable.hpp"

namespace ntt::script
{
    using namespace ecs;

    struct NativeScriptComponent;

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

        void InitSystem() override;
        void InitEntity(entity_id_t id) override;
        void Update(f32 delta, entity_id_t id) override;
        void ShutdownEntity(entity_id_t id) override;
        void ShutdownSystem() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt::script
