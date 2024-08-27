#include <NTTEngine/core/Logger.hpp>

namespace ntt::log
{
    class LoggerImpl : public Logger
    {
    public:
        LoggerImpl(const char *name) : m_name(name) {};
        virtual ~LoggerImpl() = default;

        void Log(LogLevel level, const char *file,
                 u16 line,
                 const char *message, ...) override;

        inline void SetLevel(LogLevel level) override { m_level = level; }
        void SetFormat(const char *format) override;

    private:
        const char *m_name;
        LogLevel m_level = LogLevel::LOG_INFO;
        const char *m_format = "[%l] - %t - %f:%L - %m";
    };
} // namespace ntt::log
