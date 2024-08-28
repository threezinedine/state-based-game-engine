#include "Logger.hpp"
#include <chrono>
#include <filesystem>
#include "Handler.hpp"

namespace ntt::log
{
    Logger::Logger(const char *name, LogLevel level, u8 type, const char *format)
        : m_name(name), m_level(level), m_format(format)
    {
        // TODO: Implement logger format checking
        ExtractHandlers(type, m_handlers);
    }

    void Logger::Log(LogLevel level, const char *file, u16 line, const char *message)
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

        const LogMessage logMessage = {
            levelStrs[level],
            fileName.c_str(),
            line,
            message,
            timeStr};

        for (auto &handler : m_handlers)
        {
            handler->Handle(m_name, logMessage, m_format);
        }
    }
} // namespace ntt::log