#include <NTTEngine/core/logging/ConsoleHandler.hpp>
#include <NTTEngine/platforms/stream.hpp>

namespace ntt::log
{
    using namespace platforms;

    ConsoleHandler::ConsoleHandler()
    {
    }

    void ConsoleHandler::Handle(const LogMessage &message)
    {
        TerminalColor color = TerminalColor::WHITE;

        switch (message.level)
        {
        case LogLevel::DEBUG:
            color = TerminalColor::LIGHT_CYAN;
            break;
        case LogLevel::INFO:
            color = TerminalColor::LIGHT_GREEN;
            break;
        case LogLevel::WARN:
            color = TerminalColor::LIGHT_YELLOW;
            break;
        case LogLevel::ERROR:
            color = TerminalColor::LIGHT_RED;
            break;
        case LogLevel::FATAL:
            color = TerminalColor::LIGHT_PURPLE;
            break;
        }

        Print(color, message.fullMessage.RawString().c_str());
    }
} // namespace ntt::log
