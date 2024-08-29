#include "Logger.hpp"
#include <chrono>
#include <filesystem>

namespace ntt::log
{
    Logger::Logger(const char *name, LogLevel level,
                   LoggerType type, const char *format)
        : m_name(name), m_level(level), m_format(format)
    {
        // TODO: Implement logger format checking
        ExtractHandlers(type, m_handlers);
    }

    void Logger::Log(LogLevel level, const char *file, u16 line, const char *message)
    {
        if (level > m_level)
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

        LogMessage logMessage = {
            m_name,
            level,
            levelStrs[level],
            fileName.c_str(),
            line,
            message,
            timeStr,
        };

        logMessage.fullMessage = m_format;
        logMessage.fullMessage.Replace("@l", logMessage.levelStr);
        logMessage.fullMessage.Replace("@n", logMessage.name);
        logMessage.fullMessage.Replace("@f", logMessage.fileName);
        logMessage.fullMessage.Replace("@L", std::to_string(logMessage.line));
        logMessage.fullMessage.Replace("@m", logMessage.message);
        logMessage.fullMessage.Replace("@t", logMessage.time);
        logMessage.fullMessage += "\n";

        for (auto &handler : m_handlers)
        {
            handler->Handle(logMessage);
        }
    }
} // namespace ntt::log