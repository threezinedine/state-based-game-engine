#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt
{
    void EditorLogWindowInit();
    void EditorLogWindowUpdate(b8 *p_open, b8 isRunning);
    void AppendLog(const char *log);
    void EditorLogWindowShutdown();
} // namespace ntt
