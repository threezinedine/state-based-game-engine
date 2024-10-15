#include <NTTEngine/editor/editor_log_handler.hpp>
#include <NTTEngine/editor/editor.hpp>
#include "editor_windows/log_window/log_window.hpp"
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    EditorLogHandler::EditorLogHandler()
    {
        PROFILE_FUNCTION();
    }

    EditorLogHandler::~EditorLogHandler()
    {
        PROFILE_FUNCTION();
    }

    void EditorLogHandler::Handle(const LogMessage &message)
    {
        PROFILE_FUNCTION();
        AppendLog(message.fullMessage.RawString().c_str());
    };
} // namespace ntt
