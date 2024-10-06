#pragma once
#include "logging/logging.hpp"

#define NTT_ENABLE_ASSERTS

// define the debug break
#ifdef PLATFORM_WINDOWS
// #define DEBUG_BREAK() __debugbreak()
#define DEBUG_BREAK()
#else
#define DEBUG_BREAK()
#endif

#ifdef NTT_ENABLE_ASSERTS
#define ASSERT(exp)                                     \
    if (!(exp))                                         \
    {                                                   \
        NTT_ENGINE_FATAL("Assertion failed: %s", #exp); \
    }

#define ASSERT_M(exp, msg)     \
    if (!(exp))                \
    {                          \
        NTT_ENGINE_FATAL(msg); \
    }

#define ASSERT_STA(exp, sta) \
    if (!(exp))              \
    {                        \
        sta;                 \
    }
#else
#define ASSERT(exp)
#define ASSERT_M(exp, msg)
#define ASSERT_STA(exp, sta)
#endif
