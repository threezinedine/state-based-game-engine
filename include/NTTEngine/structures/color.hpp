#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt
{
    struct Color
    {
        u8 r, g, b, a;

        Color(u8 r = 0, u8 g = 0, u8 b = 0, u8 a = 255)
            : r(r), g(g), b(b), a(a)
        {
        }
    };

#define NTT_BLACK Color(0, 0, 0, 255)
#define NTT_WHITE Color(255, 255, 255, 255)
#define NTT_RED Color(255, 0, 0, 255)
#define NTT_GREEN Color(0, 255, 0, 255)
#define NTT_BLUE Color(0, 0, 255, 255)
#define NTT_YELLOW Color(255, 255, 0, 255)
#define NTT_MAGENTA Color(255, 0, 255, 255)
#define NTT_CYAN Color(0, 255, 255, 255)
#define NTT_TRANSPARENT Color(0, 0, 0, 0)
} // namespace ntt
