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
        inline void SetType(LoggerType type) override { m_type = type; }
        void SetFormat(const char *format) override;

    private:
        // This name must be different for each logger
        //      to distinguish between them
        const char *m_name;

        LogLevel m_level = LogLevel::LOG_INFO;
        LoggerType m_type = LoggerType::CONSOLE;

        // The format must always contain the following: `%l`, `%t`, `%f`, `%L`, `%m`, `%n`
        //      and can contain other characters
        //      but any other characters which are behind `%` will
        //      be error
        const char *m_format = "[%l] - %t - %f:%L - %m";
    };
} // namespace ntt::log
