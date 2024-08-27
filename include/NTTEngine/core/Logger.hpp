#pragma once
#include <NTTEngine/defines.hpp>
#include "Memory.hpp"

/**
 * Log module provides functionalities for presenting, storing and managing
 *      events of the whole engine or game application.
 * Log module has 2 separated loggers:
 *      - Engine Log: Keep track of engine events, errors, warnings, etc.
 *          (user cannot use this logger)
 *      - Game Log: Keep track the event from the game application
 *          (used by the game developer).
 *
 * The Log module also provides the way to configure the log level
 *      and format of the output message
 *
 * Not support:
 *      - //TODO: Multi-threading
 *      - //TODO: File logging
 */
namespace ntt::log
{
    using namespace memory;

    /**
     * The LogLevel represents the importance of the message
     *      in log system.
     */
    enum LogLevel
    {
        LOG_FATAL = 0, ///< The error that crashes the whole application
        LOG_ERROR = 1, ///< The error that can be handled but the application
                       ///<     cannot continue to run (like network error, etc.)
        LOG_WARN = 2,  ///< The exceptions which can be handled easily or ignored (
                       ///<     like missing resources, etc.)
        LOG_INFO = 3,  ///< The normal information such as intialized the window, etc.
        LOG_DEBUG = 4, ///< The detail information which can be valuable for debugging
                       ///<     such as the position of the object, size of the image, etc.
        LOG_TRACE = 5  ///< The information which is representing the flow of the application
                       ///<     such as the function is called, etc. (should use in profiling)
    };

    /**
     * The logger type which is supported by the logging module
     *     (can be extended in the future) such as:
     *      - Console logger
     *      - //TODO: File logger
     *
     * Each logger can have multiple types of output (like console, file, etc.)
     *
     * But currently, the logger only supports the console logger and cannot
     *      has multiple logger types. (//TODO: multiple logger types)
     */
    enum LoggerType
    {
        CONSOLE ///< Print the message to the console

        /**
         * If add more logger types:
         *     - Add the new type here
         */
    };

    /**
     * The Instance of Logger will receive the
     *      message and handles it (can be stored, printed, etc.)
     *
     * The default message format is:
     *      "[%l] - %t - %f:%L - %m"
     *
     * The default logger level is `LOG_INFO`
     * The default logger type is `CONSOLE`
     */
    class Logger
    {
    public:
        virtual ~Logger() = default;

        /**
         * Method receives the needed information and handles it
         *      inside the logger.
         *
         * @param level: The importance of the message
         * @param file: The full path of file name which the
         *      log is placed
         * @param line: The line in the file where the log is
         *      placed
         * @param message: The message which needs to be handled, this
         *      message can be formatted with the format of printf,
         *      the rest of the arguments will be passed to the function
         */
        virtual void Log(LogLevel level,
                         const char *file,
                         u16 line,
                         const char *message, ...) = 0;

        /**
         * Change the log level
         *
         * @param level: The threshold level which all messages
         *      with higher level will be printed, stored, ...
         */
        virtual void SetLevel(LogLevel level) = 0;

        /**
         * Configure the format of the of the logger
         *      with certain information:
         *      - `%l`: The level of the message
         *      - `%n`: The name of the logger
         *      - `%f`: The file name where the log is placed (only file name)
         *      - `%L`: The line where the log is placed
         *      - `%m`: The message which needs to be handled (arguments
         *          will be replaced with the format of printf)
         *      - `%t`: The time when the log is placed (format: DD/MM/YYYY HH:MM:SS)
         *
         * For example: "[%l] - %t - %f:%L - %m" ->
         *      "[INFO] - 12/12/2021 12:12:12 - main.cpp:12 - The message"
         *
         * @param format: The format of the message,
         *      if the format is invalid, the logger will not change
         *      and the warning will be printed
         */
        virtual void SetFormat(const char *format) = 0;

        /**
         * Set the logger type for the logger
         *
         * @param type: The type of the logger
         */
        virtual void SetType(LoggerType type) = 0;
    };

    /**
     * The Singleton function which is used for retrieving the
     *      instance of the logger, the logger can be any string
     *      and the logger will be created if it does not exist.
     *
     * @param loggerName: The name of the logger which needs to be retrieved
     *
     * @return The instance of the logger (cannot be null)
     */
    Ref<Logger> GetLogger(const char *loggerName);
} // namespace ntt::log

#define ENGINE_LOGGER_NAME "Engine"
#define APP_LOGGER_NAME "Application"

#define NTT_ENGINE_LEVEL(level) \
    GetLogger(ENGINE_LOGGER_NAME)->SetLevel(level)
#define NTT_ENGINE_FORMAT(format) \
    GetLogger(ENGINE_LOGGER_NAME)->SetFormat(format)
#define NTT_ENGINE_TYPE(type) \
    GetLogger(ENGINE_LOGGER_NAME)->SetType(type)

#define NTT_ENGINE_FATAL(message, ...) \
    GetLogger(ENGINE_LOGGER_NAME)->Log(LogLevel::LOG_FATAL, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_ENGINE_ERROR(message, ...) \
    GetLogger(ENGINE_LOGGER_NAME)->Log(LogLevel::LOG_ERROR, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_ENGINE_WARN(message, ...) \
    GetLogger(ENGINE_LOGGER_NAME)->Log(LogLevel::LOG_WARN, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_ENGINE_INFO(message, ...) \
    GetLogger(ENGINE_LOGGER_NAME)->Log(LogLevel::LOG_INFO, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_ENGINE_DEBUG(message, ...) \
    GetLogger(ENGINE_LOGGER_NAME)->Log(LogLevel::LOG_DEBUG, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_ENGINE_TRACE(message, ...) \
    GetLogger(ENGINE_LOGGER_NAME)->Log(LogLevel::LOG_TRACE, __FILE__, __LINE__, message, ##__VA_ARGS__)

#define NTT_APP_LEVEL(level) \
    GetLogger(APP_LOGGER_NAME)->SetLevel(level)
#define NTT_APP_FORMAT(format) \
    GetLogger(APP_LOGGER_NAME)->SetFormat(format)
#define NTT_APP_TYPE(type) \
    GetLogger(APP_LOGGER_NAME)->SetType(type)

#define NTT_APP_FATAL(message, ...) \
    GetLogger(APP_LOGGER_NAME)->Log(LogLevel::LOG_FATAL, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_APP_ERROR(message, ...) \
    GetLogger(APP_LOGGER_NAME)->Log(LogLevel::LOG_ERROR, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_APP_WARN(message, ...) \
    GetLogger(APP_LOGGER_NAME)->Log(LogLevel::LOG_WARN, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_APP_INFO(message, ...) \
    GetLogger(APP_LOGGER_NAME)->Log(LogLevel::LOG_INFO, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_APP_DEBUG(message, ...) \
    GetLogger(APP_LOGGER_NAME)->Log(LogLevel::LOG_DEBUG, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_APP_TRACE(message, ...) GetLogger(APP_LOGGER_NAME)->Log(LogLevel::LOG_TRACE, __FILE__, __LINE__, message, ##__VA_ARGS__)
