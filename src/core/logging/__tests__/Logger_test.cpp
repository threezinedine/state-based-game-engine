#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/core/logging.hpp>
#include "../handlers/TestingHandler.hpp"

using namespace ntt::log;

class LoggerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        LogInit();
    }

    void TearDown() override
    {
        LogShutdown();
    }
};

TEST_F(LoggerTest, LogFunctionShouldCall_WhenLogLevelValid)
{
    NTT_ENGINE_CONFIG(LogLevel::DEBUG, LOGGER_TESTING);

    NTT_ENGINE_TRACE("This is a trace message");
    NTT_ENGINE_DEBUG("This is a debug message");
    NTT_ENGINE_INFO("This is an info message");
    NTT_ENGINE_WARN("This is a warning message: {}", 8);
    NTT_ENGINE_ERROR("This is an error message");
    NTT_ENGINE_FATAL("This is a fatal message");

    EXPECT_THAT(GetData().HandleTimesCalled_Trace, ::testing::Eq(0));
    EXPECT_THAT(GetData().HandleTimesCalled_Debug, ::testing::Eq(1));
    EXPECT_THAT(GetData().HandleTimesCalled_Info, ::testing::Eq(1));
    EXPECT_THAT(GetData().HandleTimesCalled_Warn, ::testing::Eq(1));
    EXPECT_THAT(GetData().HandleTimesCalled_Error, ::testing::Eq(1));
    EXPECT_THAT(GetData().HandleTimesCalled_Fatal, ::testing::Eq(1));
}

TEST_F(LoggerTest, MatchTheFormat)
{
    NTT_ENGINE_CONFIG(LogLevel::DEBUG, LOGGER_TESTING, "[@l] - @m");
    NTT_ENGINE_DEBUG("This is a debug message");
    EXPECT_TRUE(
        GetData().LogMessageString ==
        "[DEBUG] - This is a debug message\n");

    NTT_ENGINE_CONFIG(LogLevel::INFO, LOGGER_TESTING, "[@l] - @t - @m");
    NTT_ENGINE_INFO("This is a info message");
    EXPECT_TRUE(
        GetData().LogMessageString.MatchPattern(
            "[INFO] - @ - This is a info message\n"));
}