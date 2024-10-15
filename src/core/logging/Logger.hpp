#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/logging/Handler.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/list.hpp>

namespace ntt
{
    /**
     * The Instance of Logger will receive the
     *      message and handles it (can be stored, printed, etc.)
     *
     * The default message format is:
     *      "[@l] - @t - @f:@L - @m"
     *
     * The default logger level is `INFO`
     * The default logger type is `CONSOLE`
     */
    class Logger
    {
    public:
        Logger(const char *name, LogLevel level,
               List<Ref<Handler>> handlers, const char *format);
        ~Logger();

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
                 const String &message);

    private:
        // This name must be different for each logger
        //      to distinguish between them
        const char *m_name;

        LogLevel m_level = LogLevel::INFO;

        // TODO: Change not use std:vector, use self-implemented list
        // all logger type which the log message
        //      will be handled through
        List<Ref<Handler>> m_handlers;

        // The format must always contain the following:
        //      `@l`, `@t`, `@f`, `@L`, `@m`, `@n`
        //      and can contain other characters
        //      but only the above characters will be replaced
        const char *m_format = "[@l] - @t - @f:@L - @m";
    };

} // namespace ntt
