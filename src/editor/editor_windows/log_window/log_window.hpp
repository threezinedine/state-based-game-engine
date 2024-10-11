#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>
#include "../OpenClosableWindow.hpp"

namespace ntt
{
    using namespace memory;

    class LogWindow : public OpenClosableWindow
    {
    public:
        LogWindow();
        ~LogWindow() override;

    protected:
        void InitImpl() override;
        void UpdateImpl(b8 *p_open) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };

    void AppendLog(const char *log);
} // namespace ntt
