#include "ConsoleHandler.hpp"
#include <cstdio>

namespace ntt::log
{
    void ConsoleHandler::Handle(const LogMessage &message)
    {
        printf(message.fullMessage);
    }
} // namespace ntt::log
