#include "TestingHandler.hpp"

namespace ntt::log
{
    TestingHandler *TestingHandler::s_instance = nullptr;

    TestingHandler::TestingHandler()
    {
        s_instance = this;
    }

    void TestingHandler::Handle(const LogMessage &message)
    {
        switch (message.level)
        {
        case LogLevel::TRACE:
            HandleTimesCalled_Trace++;
            break;
        case LogLevel::DEBUG:
            HandleTimesCalled_Debug++;
            break;
        case LogLevel::INFO:
            HandleTimesCalled_Info++;
            break;
        case LogLevel::WARN:
            HandleTimesCalled_Warn++;
            break;
        case LogLevel::ERROR:
            HandleTimesCalled_Error++;
            break;

        default:
            HandleTimesCalled_Fatal++;
        }
    }
} // namespace ntt::log
