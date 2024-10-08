#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/application/script_system/native_script.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace script;
    using namespace memory;

    class MoveAroundController : public Script
    {
    public:
        MoveAroundController(void *);
        ~MoveAroundController() override;

    protected:
        void OnEnterImpl() override;
        void OnExitImpl() override;
        void OnUpdateImpl(f32 deltaTime) override;

        void OnHoverEnter() override;
        void OnHover(HoveringContext &) override;
        void OnHoverExit() override;
        void OnEvent(event_code_t, void *, const EventContext &) override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
