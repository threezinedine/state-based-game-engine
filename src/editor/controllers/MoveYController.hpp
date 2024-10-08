#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/application/script_system/native_script.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace script;
    using namespace memory;

    class MoveYController : public Script
    {
    public:
        MoveYController(void *);
        ~MoveYController() override;

    protected:
        void OnEnterImpl() override;
        void OnExitImpl() override;
        void OnUpdateImpl(f32 deltaTime) override;

        void OnHover(HoveringContext &) override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
