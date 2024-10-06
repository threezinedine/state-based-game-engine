#pragma once
#include <NTTEngine/core/logging/Handler.hpp>
#include <NTTEngine/structures/string.hpp>

namespace ntt::log
{
    struct TestingData
    {
        String LogMessageString;
        u8 HandleTimesCalled_Trace = 0;
        u8 HandleTimesCalled_Debug = 0;
        u8 HandleTimesCalled_Info = 0;
        u8 HandleTimesCalled_Warn = 0;
        u8 HandleTimesCalled_Error = 0;
        u8 HandleTimesCalled_Fatal = 0;
    };

    /**
     * This handler is used only for testing purpose, it has some
     *      information leakage, so it should not be used in the
     *      production environment
     */
    class TestingHandler : public Handler
    {
    public:
        TestingHandler();
        ~TestingHandler() override = default;

        void Handle(const LogMessage &message) override;
    };

    /**
     * Get the test data from the
     *    latest call of the TestingHandler
     */
    TestingData &GetData();
} // namespace ntt::log
