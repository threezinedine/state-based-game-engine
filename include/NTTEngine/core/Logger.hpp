#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/pointers.hpp>

namespace ntt
{
    enum LogLevel
    {
        LOG_FATAL = 0,
        LOG_ERROR = 1,
        LOG_WARNING = 2,
        LOG_INFO = 3,
        LOG_DEBUG = 4,
        LOG_TRACE = 5
    };

#define DEFAULT_LOG_LEVEL LogLevel::LOG_INFO

    class Logger
    {
    public:
        virtual ~Logger() = default;
        virtual void Log(LogLevel level, const char *message, ...) = 0;
        virtual void SetLevel(LogLevel level) = 0;
    };

    Ref<Logger> GetLogger();
} // namespace ntt

#ifdef _DEBUG
#define LOG_TRACE() ntt::GetLogger()->SetLevel(ntt::LogLevel::LOG_TRACE)
#define LOG_DEBUG() ntt::GetLogger()->SetLevel(ntt::LogLevel::LOG_DEBUG)
#else
#define LOG_TRACE()
#define LOG_DEBUG()
#endif
#define LOG_INFO() ntt::GetLogger()->SetLevel(ntt::LogLevel::LOG_INFO)
#define LOG_WARNING() ntt::GetLogger()->SetLevel(ntt::LogLevel::LOG_WARNING)
#define LOG_ERROR() ntt::GetLogger()->SetLevel(ntt::LogLevel::LOG_ERROR)
#define LOG_FATAL() ntt::GetLogger()->SetLevel(ntt::LogLevel::LOG_FATAL)

#define ENGINE_FATAL(message, ...) GetLogger()->Log(LogLevel::LOG_FATAL, message, __VA_ARGS__)
#define ENGINE_ERROR(message, ...) GetLogger()->Log(LogLevel::LOG_ERROR, message, __VA_ARGS__)
#define ENGINE_WARNING(message, ...) GetLogger()->Log(LogLevel::LOG_WARNING, message, __VA_ARGS__)
#define ENGINE_INFO(message, ...) GetLogger()->Log(LogLevel::LOG_INFO, message, ##__VA_ARGS__)
#define ENGINE_DEBUG(message, ...) GetLogger()->Log(LogLevel::LOG_DEBUG, message, __VA_ARGS__)
#define ENGINE_TRACE(message, ...) GetLogger()->Log(LogLevel::LOG_TRACE, message, __VA_ARGS__)

#define USER_FATAL(message, ...) ntt::GetLogger() \
                                     ->Log(ntt::LogLevel::LOG_FATAL, message, __VA_ARGS__)
#define USER_ERROR(message, ...) ntt::GetLogger() \
                                     ->Log(ntt::LogLevel::LOG_ERROR, message, __VA_ARGS__)
#define USER_WARNING(message, ...) ntt::GetLogger() \
                                       ->Log(ntt::LogLevel::LOG_WARNING, message, __VA_ARGS__)
#define USER_INFO(message, ...) ntt::GetLogger() \
                                    ->Log(ntt::LogLevel::LOG_INFO, message, __VA_ARGS__)
#define USER_DEBUG(message, ...) ntt::GetLogger() \
                                     ->Log(ntt::LogLevel::LOG_DEBUG, message, __VA_ARGS__)
#define USER_TRACE(message, ...) ntt::GetLogger() \
                                     ->Log(ntt::LogLevel::LOG_TRACE, message, __VA_ARGS__)
