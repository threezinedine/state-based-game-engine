#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/core/logging.hpp>
#include "../handlers/TestingHandler.hpp"

using namespace ntt::log;

class LoggerTest : public ::testing::Test
{
protected:
};

TEST_F(LoggerTest, LogFunctionShouldCall_WhenLogLevelValid)
{
    NTT_ENGINE_CONFIG(LogLevel::DEBUG, LOGGER_TESTING);

    NTT_ENGINE_TRACE("This is a trace message");
    NTT_ENGINE_DEBUG("This is a debug message");
    NTT_ENGINE_INFO("This is an info message");
    NTT_ENGINE_WARN("This is a warning message: %d", 8);
    NTT_ENGINE_ERROR("This is an error message");
    NTT_ENGINE_FATAL("This is a fatal message");

    EXPECT_THAT(TestingHandler::s_instance->HandleTimesCalled_Trace, ::testing::Eq(0));
    EXPECT_THAT(TestingHandler::s_instance->HandleTimesCalled_Debug, ::testing::Eq(1));
    EXPECT_THAT(TestingHandler::s_instance->HandleTimesCalled_Info, ::testing::Eq(1));
    EXPECT_THAT(TestingHandler::s_instance->HandleTimesCalled_Warn, ::testing::Eq(1));
    EXPECT_THAT(TestingHandler::s_instance->HandleTimesCalled_Error, ::testing::Eq(1));
    EXPECT_THAT(TestingHandler::s_instance->HandleTimesCalled_Fatal, ::testing::Eq(1));
}