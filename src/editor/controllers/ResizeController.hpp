#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/application/script_system/native_script.hpp>
#include <NTTEngine/core/memory.hpp>
#include <functional>

namespace ntt
{
    using namespace script;
    using namespace memory;

    using OnResizeFunc = std::function<void(
        const f32,
        const f32,
        Ref<Geometry>,
        Ref<Parent>)>;
    using OnResizeMainFunc = std::function<void(
        const f32,
        const f32,
        Ref<Geometry>)>;

    struct ResizeControllerData
    {
        entity_id_t entity;
        OnResizeFunc onResize;
        OnResizeMainFunc onResizeMain;
    };

    class ResizeController : public Script
    {
    public:
        ResizeController(void *);
        ~ResizeController() override;

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