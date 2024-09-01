#include "ApplicationImpl.hpp"
#include <raylib.h>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>

namespace ntt
{
    using namespace memory;
    using namespace log;
    using namespace event;
    using namespace renderer;

    ApplicationImpl::ApplicationImpl(u16 screenWidth, u16 screenHeight,
                                     const char *title, const Phrases &phrases)
        : m_screenWidth(screenWidth), m_screenHeight(screenHeight),
          m_title(title), m_phrases(phrases)
    {
        NTT_ENGINE_CONFIG(LogLevel::DEBUG, LOGGER_CONSOLE);
        MemoryInit();
    }

    ApplicationImpl::~ApplicationImpl()
    {
    }

    void ApplicationImpl::Begin()
    {
        NTT_ENGINE_INFO("Starting the application ...");
        SetTraceLogLevel(LOG_NONE);
        // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(m_screenWidth, m_screenHeight, m_title);
        SetTargetFPS(60);
        RendererInit();
        m_phrases.Begin();
        NTT_ENGINE_INFO("The application is started.");

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

        auto delta = static_cast<f32>(m_timer.GetMilliseconds());
        m_timer.Reset();

        input::Update(delta);
        BeginDrawing();
        ClearBackground(BLACK);
        m_phrases.MainLoop(delta);
        EndDrawing();
    }

    void ApplicationImpl::End()
    {
        NTT_ENGINE_INFO("The applicaiton is closing ...");
        m_phrases.Close();
        RendererShutdown();
        CloseWindow();
        NTT_ENGINE_INFO("The applicaiton is closed");
        MemoryShutdown();
        LogShutdown();
    }

    Scope<Application> CreateApplication(u16 screenWidth, u16 screenHeight, const char *title,
                                         const Phrases &phrases)
    {
        return CreateScope<ApplicationImpl>(screenWidth, screenHeight, title, phrases);
    }
} // namespace ntt
