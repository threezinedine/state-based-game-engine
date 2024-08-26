#include "Application.hpp"
#include <raylib.h>

namespace ntt
{
    Application::Application(u16 screenWidth, u16 screenHeight, const char *title)
        : m_screenWidth(screenWidth), m_screenHeight(screenHeight), m_title(title)
    {
    }

    Application::~Application()
    {
    }

    void Application::Begin()
    {
        InitWindow(m_screenWidth, m_screenHeight, m_title);
        SetTargetFPS(60);
    }

    bool Application::ShouldClose()
    {
        return WindowShouldClose();
    }

    void Application::Update(f32 deltaTime)
    {
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        EndDrawing();
    }

    void Application::End()
    {
        CloseWindow();
    }

    Scope<IApplication> CreateApplication(u16 screenWidth, u16 screenHeight, const char *title)
    {
        return CreateScope<Application>(screenWidth, screenHeight, title);
    }
} // namespace ntt
