#pragma once
#include <NTTEngine/defines.hpp>

#ifdef PLATFORM_WINDOWS
#include <raylib.h>

#define CREATE_WINDOW(width, height, title) \
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);  \
    SetTraceLogLevel(LOG_NONE);             \
    InitWindow(width, height, title);       \
    MaximizeWindow();                       \
    SetTargetFPS(60)
#define WINDOW_SHOULD_CLOSE() (WindowShouldClose())
#define BEGIN_DRAWING() BeginDrawing()
#define END_DRAWING() EndDrawing()
#define CLOSE_WINDOW() CloseWindow()

#else
#error "Platform not supported"
#endif
