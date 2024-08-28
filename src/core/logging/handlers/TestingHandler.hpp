#pragma once
#include "Handler.hpp"

namespace ntt::log
{
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

        static TestingHandler *s_instance;

        /**
         * Use this method after each test to reset the
         */
        inline static void DeleteInstance()
        {
            delete s_instance;
            s_instance = nullptr;
        }

        /**
         * These variables are not needed to be
         *      reset after each test
         */

        u8 HandleTimesCalled_Trace = 0;
        u8 HandleTimesCalled_Debug = 0;
        u8 HandleTimesCalled_Info = 0;
        u8 HandleTimesCalled_Warn = 0;
        u8 HandleTimesCalled_Error = 0;
        u8 HandleTimesCalled_Fatal = 0;
    };
} // namespace ntt::log
