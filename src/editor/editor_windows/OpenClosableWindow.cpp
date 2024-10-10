#include "OpenClosableWindow.hpp"
#include "imgui.h"

namespace ntt
{
    class OpenClosableWindow::Impl
    {
    public:
        b8 isRunning = FALSE;
    };

    OpenClosableWindow::OpenClosableWindow()
        : m_impl(CreateScope<Impl>())
    {
    }

    OpenClosableWindow::~OpenClosableWindow()
    {
    }

    void OpenClosableWindow::Init()
    {
        InitImpl();
    }

    void OpenClosableWindow::Update()
    {
        if (!m_impl->isRunning)
        {
            return;
        }

        UpdateImpl(&(m_impl->isRunning));
    }

    void OpenClosableWindow::Shutdown()
    {
        ShutdownImpl();
    }

    void OpenClosableWindow::DrawMenuItem()
    {
        ImGui::MenuItem("Log", NULL, &(m_impl->isRunning));
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