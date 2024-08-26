#pragma once
#include <NTTEngine/core/Logger.hpp>

namespace ntt
{
    class LoggerImpl : public Logger
    {
    public:
        LoggerImpl();
        virtual ~LoggerImpl() override;
        virtual void Log(LogLevel level, const char *message, ...) override;
        inline void SetLevel(LogLevel level) override { m_LogLevel = level; }

    private:
        LogLevel m_LogLevel = DEFAULT_LOG_LEVEL;
    };
} // namespace ntt
