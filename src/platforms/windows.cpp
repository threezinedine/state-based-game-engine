#include <NTTEngine/platforms/platforms.hpp>
#include <cstdio>
#include <cstdarg>

#ifdef PLATFORM_WINDOWS

#include <Windows.h>

namespace ntt::platforms
{
    void Print(Color color, const char *message, ...)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        int c = 0;

#define WINDOW_COLOR(color, value) \
    case Color::color:             \
        c = value;                 \
        break;

        switch (color)
        {
            WINDOW_COLOR(BLACK, 0);
            WINDOW_COLOR(BLUE, 1);
            WINDOW_COLOR(GREEN, 2);
            WINDOW_COLOR(CYAN, 3);
            WINDOW_COLOR(RED, 4);
            WINDOW_COLOR(PURPLE, 5);
            WINDOW_COLOR(YELLOW, 6);
            WINDOW_COLOR(GRAY, 8);
            WINDOW_COLOR(LIGHT_BLUE, 9);
            WINDOW_COLOR(LIGHT_GREEN, 10);
            WINDOW_COLOR(LIGHT_CYAN, 11);
            WINDOW_COLOR(LIGHT_RED, 12);
            WINDOW_COLOR(LIGHT_PURPLE, 13);
            WINDOW_COLOR(LIGHT_YELLOW, 14);
            WINDOW_COLOR(LIGHT_WHITE, 15);
        default:
            c = 7; // white
        }

        SetConsoleTextAttribute(hConsole, c);
        va_list args;
        va_start(args, message);
        vprintf(message, args);
        va_end(args);
        SetConsoleTextAttribute(hConsole, WHITE);
    }
} // namespace ntt::platforms

#endif