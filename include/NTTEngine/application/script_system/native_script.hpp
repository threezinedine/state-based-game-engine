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
} // namespace ntt::script
