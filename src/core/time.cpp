#include <NTTEngine/core/time.hpp>
#include <chrono>

namespace ntt
{
    class Timer::Impl
    {
    public:
        Impl() = default;
        ~Impl() = default;

        void Reset()
        {
            m_start = std::chrono::high_resolution_clock::now();
        }

        i64 GetMicroseconds() const
        {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - m_start);
            return duration.count();
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    };

    Timer::Timer()
    {
        m_impl = memory::CREATE_SCOPE(Impl);
        Reset();
    }

    Timer::~Timer()
    {
    }

    void Timer::Reset()
    {
        m_impl->Reset();
    }

    timer_t Timer::GetSeconds() const
    {
        return static_cast<timer_t>((timer_t)GetMilliseconds() / 10000000);
    }

    timer_t Timer::GetMilliseconds() const
    {
        return static_cast<timer_t>((timer_t)m_impl->GetMicroseconds() / 1000);
    }
} // namespace ntt
