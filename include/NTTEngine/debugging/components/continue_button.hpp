#pragma once
#include <NTTEngine/application/script_system/native_script.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace script;

    class ContinueButton : public Script
    {
    public:
        ContinueButton();
        ~ContinueButton();

        void OnEnterImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
