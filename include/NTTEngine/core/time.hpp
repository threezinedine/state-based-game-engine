#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    /**
     * This type can be u32, u64, f32, f64
     *      whatever you want to use
     */
    using timer_t = f32;

    /**
     * Can be used for counting the time steps
     *      or triggering some events
     */
    class Timer
    {
    public:
        Timer();
        ~Timer();

        /**
         * All internal states will be reset
         *      into default setup, the time
         *      which get by other functions
         *      will be counted from this moment
         */
        void Reset();

        /**
         * Time from reset moment (in seconds)
         */
        timer_t GetSeconds() const;

        /**
         * Time from reset moment (in milliseconds)
         *
         */
        timer_t GetMilliseconds() const;

    private:
        class Impl;
        memory::Scope<Impl> m_impl;
    };
} // namespace ntt
