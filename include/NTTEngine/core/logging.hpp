#pragma once
#include <NTTEngine/defines.hpp>
#include "memory.hpp"

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
        FATAL = 0, ///< The error that crashes the whole application
        ERROR = 1, ///< The error that can be handled but the application
                   ///<     cannot continue to run (like network error, etc.)
        WARN = 2,  ///< The exceptions which can be handled easily or ignored (
                   ///<     like missing resources, etc.)
        INFO = 3,  ///< The normal information such as intialized the window, etc.
        DEBUG = 4, ///< The detail information which can be valuable for debugging
                   ///<     such as the position of the object, size of the image, etc.
        TRACE = 5  ///< The information which is representing the flow of the application
                   ///<     such as the function is called, etc. (should use in profiling)
    };

    /**
     * The logger type which is supported by the logging module
     *     (can be extended in the future) such as:
     *      - Console logger
     *      - //TODO: File logger
     *
     * Each logger can have multiple types of output (like console, file, etc.)
     *      which is used by | operator to combine multiple types.
     *      like: `LOGGER_CONSOLE | LOGGER_FILE`
     *
     * The default logger type is `LOGGER_CONSOLE` which is 0b00000001
     *      if the logger has multiple types, the logger will handle all types
     *      which are used in the logger. for example:
     *          - If the logger has 2 types: `LOGGER_CONSOLE | LOGGER_FILE`
     *              then type will be 0b00000011
     *          - If there is only 1 type: `LOGGER_FILE`
     *              then type will be 0b00000010
     *
     * The `0b10000000` is used for testing purpose
     * The `0b00000000` is reserved for do nothing with the message
     *
     * But currently, the logger only supports the console logger and cannot
     *      has multiple logger types. (//TODO: multiple logger types)
     *
     * <<Add new logger type>>:
     *      - Define the new logger in the format (0x01 << n)
     *      - Define the new handler class which inherits from Handler
     *      - Update the ExtractHandlers function in Handler.hpp
     *      - If the number of types is more than 8, then must
     *          increase the size of the type variable by
     *          changing the LoggerType to u16 or u32
     */

#define LOGGER_NONE 0x00    ///< The logger which does nothing with the message
#define LOGGER_CONSOLE 0x01 ///< The logger which prints the message to the console
#define LOGGER_FILE (0x01 << 1)
#define LOGGER_TESTING (0x01 << 7) ///< For testing purpose, not use in production

    using LoggerType = u8;

    /**
     * Initialize the logger with configuration
     *      each time this function is called, the logger
     *      will be created if it is not created yet,
     *      or the configuration will be updated.
     *
     * Each logger must have the unique initialization
     *      configuration, if not, the default configuration
     *      will be used (`CONSOLE`, "[@l] - @t - @f:@L - @m", `INFO`)
     *
     * @param name: Must be unique for each logger or it will raise
     *      the warning and use the default configuration.
     *
     * @param level: The threshold level which all messages
     *      with higher level will be printed, stored, ...
     *
     * @param type: The type of the logger which will handle the message
     *      (currently only support `CONSOLE`)
     *
     * @param format: The format of the of the printed message
     *      which has some special characters:
     *          - `@l`: The level of the message
     *          - `@n`: The name of the logger
     *          - `@f`: The file name where the log is placed (only file name)
     *          - `@L`: The line where the log is placed
     *          - `@m`: The message which needs to be handled (arguments will
     *              be replaced with the format of printf)
     *          - `@t`: The time when the log is placed (format: DD/MM/YYYY HH:MM:SS)
     *      The default format is "[@l] - @t - @f:@L - @m"
     */
    void ConfigureLogger(const char *name, LogLevel level = LogLevel::INFO,
                         LoggerType type = LOGGER_CONSOLE,
                         const char *format = "[@l] - @t - @f:@L - @m");

    /**
     * The needed arguments to perform the logging
     *
     * @param name: The name of the logger which is used to distinguish
     * @param level: The importance of the message
     * @param file: The full path of file name which the log is placed
     * @param line: The line in the file where the log is placed
     * @param message: The message which needs to be handled, this
     */
    void Log(const char *name, LogLevel level, const char *file,
             u16 line,
             const char *message, ...);
} // namespace ntt::log

#define ENGINE_LOGGER_NAME "Engine"
#define APP_LOGGER_NAME "Application"

#define NTT_ENGINE_CONFIG(...) \
    ConfigureLogger(ENGINE_LOGGER_NAME, ##__VA_ARGS__)

#define NTT_ENGINE_FATAL(message, ...) \
    Log(ENGINE_LOGGER_NAME, LogLevel::FATAL, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_ENGINE_ERROR(message, ...) \
    Log(ENGINE_LOGGER_NAME, LogLevel::ERROR, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_ENGINE_WARN(message, ...) \
    Log(ENGINE_LOGGER_NAME, LogLevel::WARN, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_ENGINE_INFO(message, ...) \
    Log(ENGINE_LOGGER_NAME, LogLevel::INFO, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_ENGINE_DEBUG(message, ...) \
    Log(ENGINE_LOGGER_NAME, LogLevel::DEBUG, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_ENGINE_TRACE(message, ...) \
    Log(ENGINE_LOGGER_NAME, LogLevel::TRACE, __FILE__, __LINE__, message, ##__VA_ARGS__)

#define NTT_APP_CONFIG(...) \
    ConfigureLogger(APP_LOGGER_NAME, ##__VA_ARGS__)

#define NTT_APP_FATAL(message, ...) \
    Log(APP_LOGGER_NAME, LogLevel::FATAL, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_APP_ERROR(message, ...) \
    Log(APP_LOGGER_NAME, LogLevel::ERROR, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_APP_WARN(message, ...) \
    Log(APP_LOGGER_NAME, LogLevel::WARN, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_APP_INFO(message, ...) \
    Log(APP_LOGGER_NAME, LogLevel::INFO, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_APP_DEBUG(message, ...) \
    Log(APP_LOGGER_NAME, LogLevel::DEBUG, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define NTT_APP_TRACE(message, ...) Log(APP_LOGGER_NAME, LogLevel::LOG_TRACE, __FILE__, __LINE__, message, ##__VA_ARGS__)