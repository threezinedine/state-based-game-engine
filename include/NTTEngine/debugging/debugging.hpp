#pragma once
#include <NTTEngine/defines.hpp>

/**
 * Module provides the mechanism for debugging the engine.
 * User can stop the programming at a moment, and check the states of enties, systems, etc.
 * Some other features like run next frame, reset, run until error, etc. are also provided.
 * User can also put some breakpoints into the code (BREAKPOINT macro) to stop the program
 *      at that point.
 */
namespace ntt::debugging
{
    /**
     * Initialize the debugging module.
     * Should be called at the initialization phase of the application
     */
    void DebugInit();

    /**
     * Stop the program at the current point. When this function is called,
     *      all other system (except the rendering system) will be paused.
     *
     * If the game is running, but this function is called, the debug mode
     *      will be activated automatically.
     *
     * The DEBUG_BREAK event will be triggered.
     * Other systems should has their own debug mechanism to handle this event rather
     *      than using the interface of this module.
     */
    void DebugBreak();

    /**
     * Run the program until the next Breakpoint.
     *
     * If there is no breakpoint, the program will run or turn off normally.
     * The DEBUG_CONTINUE event will be triggered and the debug mode will be deactivated.
     *      (the event context will passed a FALSE value)
     */
    void NextBreakpoint();

    /**
     * Run until the end of the next frame is reached.
     * The DEBUG_CONTINUE event will be triggered and the debug mode will still be activated.
     *      (the event context will passed a TRUE value)
     */
    void NextFrame();

    /**
     * Check whether the program hits the breakpoint or not.
     * This function currently should not be used, the stop mode should
     *      trigger the debug mode event rather than has some polling mechanism.
     *
     * @return true if the program hits the breakpoint, false otherwise
     */
    b8 DebugIsStopped();

    /**
     * Delete the debugging module.
     * Should be called at the end of the application (shutdown phase)
     */
    void DebugShutdown();
} // namespace ntt
