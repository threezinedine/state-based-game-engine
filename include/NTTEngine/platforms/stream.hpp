#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/color.hpp>

namespace ntt::platforms
{
    void Print(TerminalColor color, const char *message, ...);

    template <typename... Args>
    void Print(const char *message, Args... args)
    {
        Print(TerminalColor::WHITE, message, args...);
    }
} // namespace ntt::platforms