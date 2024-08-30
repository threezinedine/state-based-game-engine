#include "ApplicationImpl.hpp"
#include <raylib.h>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <glfw/glfw3.h>

namespace ntt
{
    using namespace memory;
    using namespace log;
    using namespace event;

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
        // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(m_screenWidth, m_screenHeight, m_title);
        SetTargetFPS(60);

        m_timer.Reset();
    }

    bool ApplicationImpl::ShouldClose()
    {
        return WindowShouldClose();
    }

    void ApplicationImpl::Update()
    {
        // if (IsWindowResized())
        // {
        //     u32 width = GetScreenWidth();
        //     u32 height = GetScreenHeight();

        //     EventContext context;

        //     context.u32_data[0] = width;
        //     context.u32_data[1] = height;
        //     TriggerEvent(EventCode::WINDOW_RESIZED, nullptr, context);
        // }

        auto delta = m_timer.GetMilliseconds();
        m_timer.Reset();

        input::Update(static_cast<f32>(delta));
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
