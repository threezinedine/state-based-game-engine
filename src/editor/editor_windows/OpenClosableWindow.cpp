#include <NTTEngine/editor/OpenClosableWindow.hpp>
#include "imgui.h"
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    class OpenClosableWindow::Impl
    {
    public:
        b8 isRunning = FALSE;
        String name;
    };

    OpenClosableWindow::OpenClosableWindow()
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
        m_impl->name = "";
    }

    OpenClosableWindow::OpenClosableWindow(const String &name)
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
        m_impl->name = name;
    }

    OpenClosableWindow::~OpenClosableWindow()
    {
        PROFILE_FUNCTION();
    }

    void OpenClosableWindow::Init()
    {
        PROFILE_FUNCTION();
        InitImpl();
    }

    void OpenClosableWindow::Update(ImGuiWindowFlags flags)
    {
        PROFILE_FUNCTION();
        if (!m_impl->isRunning)
        {
            return;
        }

        UpdateImpl(&(m_impl->isRunning), flags);
    }

    void OpenClosableWindow::Shutdown()
    {
        PROFILE_FUNCTION();
        ShutdownImpl();
    }

    void OpenClosableWindow::DrawMenuItem()
    {
        PROFILE_FUNCTION();
        ImGui::MenuItem(m_impl->name.RawString().c_str(), NULL, &(m_impl->isRunning));
    }

    void OpenClosableWindow::Open()
    {
        PROFILE_FUNCTION();
        m_impl->isRunning = TRUE;
    }

    void OpenClosableWindow::Close()
    {
        PROFILE_FUNCTION();
        m_impl->isRunning = FALSE;
    }
} // namespace ntt