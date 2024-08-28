#include <NTTEngine/core/logging.hpp>
#include "Handler.hpp"

// TODO: Temporary includes
#include <vector>
// End of temporary includes

namespace ntt::log
{
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
        Logger(const char *name, LogLevel level,
               u8 type, const char *format);
        ~Logger() = default;

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
        void Log(LogLevel level,
                 const char *file,
                 u16 line,
                 const char *message);

    private:
        // This name must be different for each logger
        //      to distinguish between them
        const char *m_name;

        LogLevel m_level = LogLevel::LOG_INFO;

        // TODO: Change not use std:vector, use self-implemented list
        // all logger type which the log message
        //      will be handled through
        std::vector<Scope<Handler>> m_handlers;

        // The format must always contain the following: `%l`, `%t`, `%f`, `%L`, `%m`, `%n`
        //      and can contain other characters
        //      but any other characters which are behind `%` will
        //      be error
        const char *m_format = "[%l] - %t - %f:%L - %m";
    };

} // namespace ntt::log
