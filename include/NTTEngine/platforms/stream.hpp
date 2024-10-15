#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/terminal_color.hpp>

namespace ntt
{
    void Print(TerminalColor color, const char *message, ...);

    template <typename... Args>
    void Print(const char *message, Args... args)
    {
        Print(TerminalColor::NTT_TERMINAL_COLOR_WHITE, message, args...);
    }
} // namespace ntt