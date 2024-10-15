#include <NTTEngine/core/logging/ConsoleHandler.hpp>
#include <NTTEngine/platforms/stream.hpp>

namespace ntt
{
    ConsoleHandler::ConsoleHandler()
    {
    }

    void ConsoleHandler::Handle(const LogMessage &message)
    {
        TerminalColor color = TerminalColor::NTT_TERMINAL_COLOR_WHITE;

        switch (message.level)
        {
        case LogLevel::DEBUG:
            color = TerminalColor::NTT_TERMINAL_COLOR_LIGHT_CYAN;
            break;
        case LogLevel::INFO:
            color = TerminalColor::NTT_TERMINAL_COLOR_LIGHT_GREEN;
            break;
        case LogLevel::WARN:
            color = TerminalColor::NTT_TERMINAL_COLOR_LIGHT_YELLOW;
            break;
        case LogLevel::ERROR:
            color = TerminalColor::NTT_TERMINAL_COLOR_LIGHT_RED;
            break;
        case LogLevel::FATAL:
            color = TerminalColor::NTT_TERMINAL_COLOR_LIGHT_PURPLE;
            break;
        }

        Print(color, message.fullMessage.RawString().c_str());
    }
} // namespace ntt
