#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/string.hpp>

namespace ntt
{
    using namespace memory;

    /**
     * Used for measuring, comparing, monitoring the performance, the efficiency of the code.
     * Each function, method, block of code can be profiled to see how much time it takes
     *      to execute. An instance of the Profiling class can be created at the start of the
     *      block of code to be profiled, and the destructor will be called automatically
     *      at the end.
     *
     * This instance must be called after the ProfilingInit() function.
     *
     * @example:
     * ```C++
     * i32 RandomInt(i32 min, i32 max)
     * {
     *      ntt::Profiling p("RandomInt", "Random.cpp", __LINE__);
     *
     *      // block of code
     *      // block of code
     *
     *      return result;
     * }
     * ```
     * The p will be destroyed at the end of the function, and the time taken to execute the
     *      can be printed to the console or saved to a file.
     */
    class Profiling
    {
    public:
        Profiling(String funcName, String file, u32 line);
        ~Profiling();

    private:
        class Impl;
        Scope<Impl> m_impl;
    };

    /**
     * The beginning setup for the profiling system.
     *
     * @param outputFolder: The folder where the profiling data will be saved.
     *      Each section will be saved in a separate file.
     */
    void ProfilingInit(String outputFolder);

    /**
     * The profiling can have multiple sections for different parts of the code.
     *      (like engine initialization, game loop, rendering, etc.)
     *
     * Do not use this function inside the engine, use in the main.hpp only
     *
     * When a new section is started, the previous section will be ended.
     * At the begin of the profiling, when no section has been started, then the
     *      default section will be used with the name "Default".
     * The output of the section will be saved in a file with the format:
     *      {sectionName}.prof.txt
     */
    void ProfilingBegin(String section);

    /**
     * The ending setup for the profiling system. Remove all the data, and clean up.
     */
    void ProfilingShutdown();
} // namespace ntt

// #define _PROFILING

#ifdef _PROFILING
#define PROFILE_FUNCTION() ntt::Profiling p_##__FUNCTION__##_##__LINE__(__FUNCTION__, __FILE__, __LINE__)
#else
#define PROFILE_FUNCTION()
#endif