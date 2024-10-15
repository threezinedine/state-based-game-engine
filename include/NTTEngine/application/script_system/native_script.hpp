#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/ecs/data_com.hpp>
#include "scriptable.hpp"
#include "script_store.hpp"
#include <NTTEngine/renderer/Hovering.hpp>

namespace ntt
{
    class Script : public Scriptable
    {
    public:
        virtual void OnEnter() override;
        virtual void OnExit() override;
        virtual void OnUpdate(f32 deltaTime) override;

    protected:
        virtual void OnHoverEnter() {}
        virtual void OnHover(HoveringContext &context) {}
        virtual void OnHoverExit() {}
        virtual void OnCollide(List<entity_id_t> others) {}
    };
} // namespace ntt
