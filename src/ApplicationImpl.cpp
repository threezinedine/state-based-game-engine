#include "ApplicationImpl.hpp"
#include <raylib.h>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>

namespace ntt
{
    using namespace memory;
    using namespace log;

    ApplicationImpl::ApplicationImpl(u16 screenWidth, u16 screenHeight, const char *title)
        : m_screenWidth(screenWidth), m_screenHeight(screenHeight), m_title(title)
    {
        NTT_ENGINE_CONFIG(LogLevel::DEBUG, LOGGER_CONSOLE);
    }

    ApplicationImpl::~ApplicationImpl()
    {
    }

    void ApplicationImpl::Begin()
    {
        NTT_ENGINE_INFO("Begin the application");
        SetTraceLogLevel(LOG_NONE);
        InitWindow(m_screenWidth, m_screenHeight, m_title);
        SetTargetFPS(60);
    }

    bool ApplicationImpl::ShouldClose()
    {
        return WindowShouldClose();
    }

    void ApplicationImpl::Update()
    {
        input::Update(0.0f);
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    void ApplicationImpl::End()
    {
        NTT_ENGINE_INFO("Close the application");
        CloseWindow();
    }

    Scope<Application> CreateApplication(u16 screenWidth, u16 screenHeight, const char *title)
    {
        return CreateScope<ApplicationImpl>(screenWidth, screenHeight, title);
    }
} // namespace ntt
