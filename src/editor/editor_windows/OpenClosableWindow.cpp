#include "OpenClosableWindow.hpp"
#include "imgui.h"

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
        m_impl->name = "";
    }

    OpenClosableWindow::OpenClosableWindow(const String &name)
        : m_impl(CreateScope<Impl>())
    {
        m_impl->name = name;
    }

    OpenClosableWindow::~OpenClosableWindow()
    {
    }

    void OpenClosableWindow::Init()
    {
        InitImpl();
    }

    void OpenClosableWindow::Update(ImGuiWindowFlags flags)
    {
        if (!m_impl->isRunning)
        {
            return;
        }

        UpdateImpl(&(m_impl->isRunning), flags);
    }

    void OpenClosableWindow::Shutdown()
    {
        ShutdownImpl();
    }

    void OpenClosableWindow::DrawMenuItem()
    {
        ImGui::MenuItem(m_impl->name.RawString().c_str(), NULL, &(m_impl->isRunning));
    }

    void OpenClosableWindow::Open()
    {
        m_impl->isRunning = TRUE;
    }

    void OpenClosableWindow::Close()
    {
        m_impl->isRunning = FALSE;
    }
} // namespace ntt