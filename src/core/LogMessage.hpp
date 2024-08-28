#pragma once
#include <NTTEngine/core/logging.hpp>

namespace ntt::log
{
    /**
     * All needed information of a log message
     */
    struct LogMessage
    {
        const char *levelStr; ///< The string representation of the log level
        const char *fileName; ///< The file name (not full path) which the log is placed
        u16 line;             ///< The line in the file where the log is placed
        const char *message;  ///< The message which needs to be handled (all arguments
                              ///< were replaced with the format of printf)
        const char *time;     ///< The time when the log is placed (format: DD/MM/YYYY HH:MM:SS)
    };
} // namespace ntt::log
