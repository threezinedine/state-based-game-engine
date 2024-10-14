#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/editor/OpenClosableWindow.hpp>
#include <NTTEngine/editor/ProjectReloadWindow.hpp>

namespace ntt
{
    using namespace memory;

    class LogWindow : public OpenClosableWindow, public ProjectReloadWindow
    {
    public:
        LogWindow();
        ~LogWindow() override;

        void OnReloadProject() override;

    protected:
        void InitImpl() override;
        void UpdateImpl(b8 *p_open, ImGuiWindowFlags flags) override;
        void ShutdownImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };

    void AppendLog(const char *log);
} // namespace ntt
