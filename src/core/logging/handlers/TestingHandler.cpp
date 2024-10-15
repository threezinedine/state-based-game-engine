#include "TestingHandler.hpp"

namespace ntt
{
    namespace
    {
        TestingData s_data;
    } // namespace

    TestingData &GetData()
    {
        return s_data;
    }

    TestingHandler::TestingHandler()
    {
        s_data.HandleTimesCalled_Trace = 0;
        s_data.HandleTimesCalled_Debug = 0;
        s_data.HandleTimesCalled_Info = 0;
        s_data.HandleTimesCalled_Warn = 0;
        s_data.HandleTimesCalled_Error = 0;
        s_data.HandleTimesCalled_Fatal = 0;
        s_data.LogMessageString = "";
    }

    void TestingHandler::Handle(const LogMessage &message)
    {
        switch (message.level)
        {
        case LogLevel::TRACE:
            s_data.HandleTimesCalled_Trace++;
            break;
        case LogLevel::DEBUG:
            s_data.HandleTimesCalled_Debug++;
            break;
        case LogLevel::INFO:
            s_data.HandleTimesCalled_Info++;
            break;
        case LogLevel::WARN:
            s_data.HandleTimesCalled_Warn++;
            break;
        case LogLevel::ERROR:
            s_data.HandleTimesCalled_Error++;
            break;

        default:
            s_data.HandleTimesCalled_Fatal++;
        }

        s_data.LogMessageString = message.fullMessage;
    }
} // namespace ntt
