#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/structures/string.hpp>

namespace ntt
{
    void EditorViewportWindowInit(f32 width, f32 height);
    void EditorViewportWindowStartDraw();
    void EditorViewportWindowUpdate(b8 *p_open, b8 isRunning);
    void EditorViewportWindowEndDraw();
    void EditorViewportWindowShutdown();
} // namespace ntt
