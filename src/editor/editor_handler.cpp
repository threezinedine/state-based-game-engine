#include <NTTEngine/editor/editor_log_handler.hpp>
#include <NTTEngine/editor/editor.hpp>
#include "editor_log_window.hpp"

namespace ntt
{
    EditorLogHandler::EditorLogHandler()
    {
    }

    EditorLogHandler::~EditorLogHandler()
    {
    }

    void EditorLogHandler::Handle(const LogMessage &message)
    {
        AppendLog(message.fullMessage.RawString().c_str());
    };
} // namespace ntt
