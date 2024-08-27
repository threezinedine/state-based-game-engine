#include "ApplicationImpl.hpp"
#include <raylib.h>
#include <NTTEngine/core/Logger.hpp>
#include <NTTEngine/core/Memory.hpp>

namespace ntt
{
    ApplicationImpl::ApplicationImpl(u16 screenWidth, u16 screenHeight, const char *title)
        : m_screenWidth(screenWidth), m_screenHeight(screenHeight), m_title(title)
    {
    }

    ApplicationImpl::~ApplicationImpl()
    {
    }

    void ApplicationImpl::Begin()
    {
        // ENGINE_INFO("Begin the application");
        SetTraceLogLevel(LOG_NONE);
        InitWindow(m_screenWidth, m_screenHeight, m_title);
        SetTargetFPS(60);
    }

    bool ApplicationImpl::ShouldClose()
    {
        return WindowShouldClose();
    }

    void ApplicationImpl::Update(f32 deltaTime)
    {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    void ApplicationImpl::End()
    {
        // ENGINE_INFO("End the application");
        CloseWindow();
    }

    Scope<Application> CreateApplication(u16 screenWidth, u16 screenHeight, const char *title)
    {
        return CreateScope<ApplicationImpl>(screenWidth, screenHeight, title);
    }
} // namespace ntt
