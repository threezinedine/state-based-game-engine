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
        event_id_t s_endFrameEvent = -1;
    } // namespace

    void DebugInit()
    {
        PROFILE_FUNCTION();

        s_isStopped = FALSE;
    }

    void DebugBreak()
    {
        PROFILE_FUNCTION();
        TriggerEvent(NTT_DEBUG_BREAK, nullptr, {0});
        s_isStopped = TRUE;
    }

    void NextBreakpoint()
    {
        PROFILE_FUNCTION();
        EventContext context;
        context.b8_data[0] = FALSE; /// the debugging mode is permanently off
        TriggerEvent(NTT_DEBUG_CONTINUE, nullptr, {0});
        s_isStopped = FALSE;
    }

    void NextFrame()
    {
        PROFILE_FUNCTION();
        s_endFrameEvent = RegisterEvent(
            NTT_END_FRAME,
            [&](...)
            {
                DebugBreak();
                if (s_endFrameEvent == -1)
                {
                    return;
                }
                UnregisterEvent(s_endFrameEvent);
                s_endFrameEvent = -1;
            });

        EventContext context;
        context.b8_data[0] = TRUE; /// the debugging mode is temporarily off
        TriggerEvent(NTT_DEBUG_CONTINUE, nullptr, {0});
        s_isStopped = FALSE;
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