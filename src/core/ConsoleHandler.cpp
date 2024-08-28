#include "ConsoleHandler.hpp"
#include <cstdio>

namespace ntt::log
{
    void ConsoleHandler::Handle(const char *name, const LogMessage &message, const char *format)
    {
        printf("[%s] - %s - %s:%d - %s\n", message.levelStr,
               message.time, message.fileName, message.line, message.message);
    }
} // namespace ntt::log
