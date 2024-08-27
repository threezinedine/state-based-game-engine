#include "LoggerImpl.hpp"
#include <cstdio>
#include <chrono>
#include <filesystem>
#include <cstdarg>

namespace ntt::log
{
    void LoggerImpl::Log(LogLevel level, const char *file, u16 line, const char *message, ...)
    {
        if (level < m_level)
        {
            return;
        }

        const char *levelStrs[] = {
            "FATAL",
            "ERROR",
            "WARN",
            "INFO",
            "DEBUG",
            "TRACE"};

        // convert time to string with format DD/MM/YYYY HH:MM:SS #TODO: move
        //      to another function or module
        auto now = std::chrono::system_clock::now();
        auto nowTime = std::chrono::system_clock::to_time_t(now);
        auto time = std::localtime(&nowTime);
        char timeStr[20];
        std::strftime(timeStr, sizeof(timeStr), "%d/%m/%Y %H:%M:%S", time);

        // get file name from path #TODO: move to file module
        std::filesystem::path filePath(file);
        std::string fileName = filePath.filename().string();

        printf("[%s] - %s - %s:%d - ", levelStrs[level],
               timeStr, fileName.c_str(), line);

        va_list args;
        va_start(args, message);
        vprintf(message, args);
        va_end(args);

        printf("\n");
    }

    void LoggerImpl::SetFormat(const char *format)
    {
        // TODO: Implement check format string

        m_format = format;
    }

    namespace
    {
        Ref<LoggerImpl> s_engineLogger = nullptr;
        Ref<LoggerImpl> s_appLogger = nullptr;
    } // namespace

    Ref<Logger> GetLogger(const char *loggerName)
    {
        if (loggerName == ENGINE_LOGGER_NAME)
        {
            if (s_engineLogger == nullptr)
            {
                s_engineLogger = CreateRef<LoggerImpl>(ENGINE_LOGGER_NAME);
            }

            return s_engineLogger;
        }
        else
        {
            if (s_appLogger == nullptr)
            {
                s_appLogger = CreateRef<LoggerImpl>(APP_LOGGER_NAME);
            }

            return s_appLogger;
        }
    }
} // namespace ntt::log
