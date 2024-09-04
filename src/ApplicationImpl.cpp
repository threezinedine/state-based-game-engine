#include "./ApplicationImpl.hpp"
#include <raylib.h>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/audio/audio.hpp>
#include <NTTEngine/ecs/ecs.hpp>

namespace ntt
{
    using namespace memory;
    using namespace log;
    using namespace event;
    using namespace renderer;
    using namespace audio;

    ApplicationImpl::ApplicationImpl(u16 screenWidth, u16 screenHeight,
                                     const char *title, const Phrases &phrases)
        : m_screenWidth(screenWidth), m_screenHeight(screenHeight),
          m_title(title), m_phrases(phrases)
    {
        LogInit();
        NTT_ENGINE_CONFIG(LogLevel::TRACE, LOGGER_CONSOLE);
        MemoryInit();
    }

    ApplicationImpl::~ApplicationImpl()
    {
        MemoryShutdown();
        LogShutdown();
    }

    void ApplicationImpl::Begin()
    {
        NTT_ENGINE_INFO("Starting the application ...");
        SetTraceLogLevel(LOG_NONE);
        // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(m_screenWidth, m_screenHeight, m_title);
        SetTargetFPS(60);
        RendererInit();
        AudioInit();
        ecs::ECSInit();
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
        auto delta = static_cast<f32>(m_timer.GetMilliseconds());
        m_timer.Reset();

        input::Update(delta);
        AudioUpdate(delta);
        BeginDrawing();
        ClearBackground(BLACK);
        ecs::ECSUpdate(delta);
        m_phrases.MainLoop(delta);
        EndDrawing();
    }

    void ApplicationImpl::End()
    {
        NTT_ENGINE_INFO("The applicaiton is closing ...");
        m_phrases.Close();
        ecs::ECSShutdown();
        AudioShutdown();
        RendererShutdown();
        CloseWindow();
        NTT_ENGINE_INFO("The applicaiton is closed");
    }

    Scope<Application> CreateApplication(u16 screenWidth, u16 screenHeight, const char *title,
                                         const Phrases &phrases)
    {
        return CREATE_SCOPE(ApplicationImpl, screenWidth, screenHeight, title, phrases);
    }
} // namespace ntt
