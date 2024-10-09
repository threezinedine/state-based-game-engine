#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt
{
    void EditorEntityWindowInit();
    void EditorEntityWindowUpdate(b8 *p_open, b8 isRunning);
    void EditorEntityWindowShutdown();
} // namespace ntt
