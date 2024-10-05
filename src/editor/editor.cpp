#include <NTTEngine/editor/editor.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

namespace ntt
{
    using namespace log;
    using namespace event;

    namespace
    {
        b8 s_useEditor = FALSE;
        b8 s_isRunning = FALSE;

        b8 s_shouldStop = FALSE;
        b8 s_shouldStart = FALSE;
    }

    void EditorInit(b8 use)
    {
        s_useEditor = use;

        if (s_useEditor)
        {
            TriggerEvent(NTT_EDITOR_STOP, {});
        }
    }

    void EditorRun()
    {
        if (!s_useEditor)
        {
            return;
        }

        if (s_isRunning)
        {
            return;
        }

        s_shouldStart = TRUE;
        s_isRunning = TRUE;
    }

    void EditorStop()
    {
        if (!s_useEditor)
        {
            return;
        }

        if (!s_isRunning)
        {
            return;
        }

        s_shouldStop = TRUE;
        s_isRunning = FALSE;
    }

    void EditorUpdate(f32 delta)
    {
        if (!s_useEditor)
        {
        }

        if (s_shouldStart)
        {
            TriggerEvent(NTT_EDITOR_START, {});
        }

        if (s_shouldStop)
        {
            TriggerEvent(NTT_EDITOR_STOP, {});
        }

        s_shouldStart = FALSE;
        s_shouldStop = FALSE;
    }

    void EditorShutdown()
    {
        NTT_ENGINE_INFO("The editor mode is shutdown");
    }
} // namespace ntt
