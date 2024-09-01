#include <NTTEngine/core/logging.hpp>
#include "Logger.hpp"
#include <NTTEngine/core/memory.hpp>
#include <cstdarg>
#include <cstring>
#include <fmt/core.h>

namespace ntt::log
{
    namespace
    {
        /**
         * Engine Logger is only used by the engine developer
         *     the warning will be raised if the user tries to use this logger (//TODO)
         */
        Scope<Logger> s_engineLogger = nullptr;

        /**
         * Application Logger is only used by the game developer
         */
        Scope<Logger> s_appLogger = nullptr;
    } // namespace

    void ConfigureLogger(const char *name, LogLevel level,
                         LoggerType type, const char *format)
    {
        if (strcmp(name, ENGINE_LOGGER_NAME) == 0)
        {
            s_engineLogger = CreateScope<Logger>(name, level, type, format);
        }
        else // if (strcmp(name, APP_LOGGER_NAME) == 0)
        {
            s_appLogger = CreateScope<Logger>(name, level, type, format);
        }
    }

    void Log(const char *name, LogLevel level, const char *file, u16 line, const String &message)
    {
        if (strcmp(name, ENGINE_LOGGER_NAME) == 0)
        {
            if (s_engineLogger == nullptr)
            {
                ConfigureLogger(ENGINE_LOGGER_NAME);
            }

            s_engineLogger->Log(level, file, line, message);
        }
        else
        {
            if (s_appLogger == nullptr)
            {
                ConfigureLogger(APP_LOGGER_NAME);
            }

            s_appLogger->Log(level, file, line, message);
        }
    }

    void LogShutdown()
    {
    }
} // namespace ntt::log
