#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/application/script_system/native_script.hpp>
#include <NTTEngine/core/memory.hpp>
#include <functional>

namespace ntt
{
    using namespace script;
    using namespace memory;

    using OnTransformFunc = std::function<void(
        const Position,
        const Position,
        Ref<Geometry>)>;

    using OnAddEntResetFunc = std::function<void(const entity_id_t)>;

    struct TransformScriptData
    {
        entity_id_t entity;
        OnTransformFunc onResizeMain;
        OnAddEntResetFunc onAddEntReset;
    };

    class TransformScript : public Script
    {
    public:
        TransformScript(void *);
        ~TransformScript() override;

    protected:
        void OnEnterImpl() override;
        void OnExitImpl() override;
        void OnUpdateImpl(f32 deltaTime) override;

        void OnHoverEnter() override;
        void OnHover(HoveringContext &) override;
        void OnHoverExit() override;
        void OnEvent(event_code_t code, void *sender, const EventContext &context) override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
