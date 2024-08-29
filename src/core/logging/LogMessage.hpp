#pragma once
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/string.hpp>

namespace ntt::log
{
    /**
     * All needed information of a log message
     */
    struct LogMessage
    {
        const char *name;     ///< The name of the logger which placed the log
        LogLevel level;       ///< The level of the log message
        const char *levelStr; ///< The string representation of the log level
        const char *fileName; ///< The file name (not full path) which the log is placed
        u16 line;             ///< The line in the file where the log is placed
        const char *message;  ///< The message which needs to be handled (all arguments
                              ///< were replaced with the format of printf)
        const char *time;     ///< The time when the log is placed (format: DD/MM/YYYY HH:MM:SS)
        String fullMessage;   ///< The full message with all arguments replaced
                              ///< by the format of printf
    };
} // namespace ntt::log
