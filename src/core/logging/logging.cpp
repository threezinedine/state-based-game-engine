#include <NTTEngine/core/logging/logging.hpp>
#include "Logger.hpp"
#include <NTTEngine/core/memory.hpp>
#include <cstdarg>
#include <cstring>
#include <cstdarg>
#include <any>
#include <vector>

namespace ntt
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

        b8 s_isInitialized = FALSE;
    } // namespace

    void LogInit()
    {
        if (s_isInitialized)
        {
            return;
        }

        s_isInitialized = TRUE;
    }

    void ConfigureLogger(const char *name, LogLevel level,
                         List<Ref<Handler>> handlers, const char *format)
    {
        if (!s_isInitialized)
        {
            return;
        }

        if (strcmp(name, ENGINE_LOGGER_NAME) == 0)
        {
            s_engineLogger = CreateScope<Logger>(name, level, handlers, format);
        }
        else // if (strcmp(name, APP_LOGGER_NAME) == 0)
        {
            s_appLogger = CreateScope<Logger>(name, level, handlers, format);
        }
    }

    void Log(const char *name,
             LogLevel level,
             const char *file,
             u16 line,
             const String &message)
    {
        if (!s_isInitialized)
        {
            return;
        }

        if (!s_isInitialized)
        {
            return;
        }

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
        if (!s_isInitialized)
        {
            return;
        }

        s_isInitialized = FALSE;

        s_appLogger.reset();
        s_engineLogger.reset();
    }
} // namespace ntt
