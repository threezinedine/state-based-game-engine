#include "ConsoleHandler.hpp"
#include <cstdio>

namespace ntt::log
{
    void ConsoleHandler::Handle(const LogMessage &message)
    {
        printf(message.fullMessage.RawString().c_str());
    }
} // namespace ntt::log
