#include <NTTEngine/debugging/debugging.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

namespace ntt::debugging
{
    using namespace log;
    using namespace memory;
    using namespace event;

    namespace
    {
        b8 s_isStopped = FALSE;
    } // namespace

    void DebugInit()
    {
        PROFILE_FUNCTION();
    }

    void DebugBreak()
    {
        PROFILE_FUNCTION();
        TriggerEvent(NTT_DEBUG_BREAK, nullptr, {0});
        b8 s_isStopped = TRUE;
    }

    void NextBreakpoint()
    {
        PROFILE_FUNCTION();
        EventContext context;
        context.b8_data[0] = FALSE; /// the debugging mode is permanently off
        TriggerEvent(NTT_DEBUG_CONTINUE, nullptr, {0});
        b8 s_isStopped = TRUE;
    }

    void NextFrame()
    {
        PROFILE_FUNCTION();
        auto event = RegisterEvent(
            NTT_END_FRAME,
            [](...)
            { DebugBreak(); });

        EventContext context;
        context.b8_data[0] = TRUE; /// the debugging mode is temporarily off
        TriggerEvent(NTT_DEBUG_CONTINUE, nullptr, {0});

        UnregisterEvent(event);
    }

    b8 DebugIsStopped()
    {
        return s_isStopped;
    }

    void DebugShutdown()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt::debugging
