#include "LoggerImpl.hpp"
#include <cstdio>
#include <cstdarg>

namespace ntt
{
    LoggerImpl::LoggerImpl()
    {
    }

    LoggerImpl::~LoggerImpl()
    {
    }

    void LoggerImpl::Log(LogLevel level, const char *message, ...)
    {
        if (level > m_LogLevel)
        {
            return;
        }
        va_list args;
        va_start(args, message);
        printf(message, args);
        va_end(args);

        printf("\n");
    }

    namespace
    {
        Ref<Logger> s_Logger = nullptr;
    } // namespace

    Ref<Logger> GetLogger()
    {
        if (!s_Logger)
        {
            s_Logger = CreateRef<LoggerImpl>();
        }

        return s_Logger;
    }
} // namespace ntt
