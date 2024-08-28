#pragma once
#include "logging.hpp"

#define NTT_ENABLE_ASSERTS

// define the debug break
#ifdef PLATFORM_WINDOWS
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK()
#endif

#ifdef NTT_ENABLE_ASSERTS
#define ASSERT(exp)                                     \
    if (!(exp))                                         \
    {                                                   \
        NTT_ENGINE_FATAL("Assertion failed: %s", #exp); \
        DEBUG_BREAK();                                  \
    }

#define ASSERT_M(exp, msg)     \
    if (!(exp))                \
    {                          \
        NTT_ENGINE_FATAL(msg); \
        DEBUG_BREAK();         \
    }
#else
#define ASSERT(exp)
#define ASSERT_M(exp, msg)
#endif
