#include "ConsoleHandler.hpp"
#include <NTTEngine/platforms/platforms.hpp>

namespace ntt::log
{
    using namespace platforms;

    void ConsoleHandler::Handle(const LogMessage &message)
    {
        Color color = Color::WHITE;

        switch (message.level)
        {
        case LogLevel::DEBUG:
            color = Color::LIGHT_CYAN;
            break;
        case LogLevel::INFO:
            color = Color::LIGHT_GREEN;
            break;
        case LogLevel::WARN:
            color = Color::LIGHT_YELLOW;
            break;
        case LogLevel::ERROR:
            color = Color::LIGHT_RED;
            break;
        case LogLevel::FATAL:
            color = Color::LIGHT_PURPLE;
            break;
        }

        Print(color, message.fullMessage.RawString().c_str());
    }
} // namespace ntt::log
