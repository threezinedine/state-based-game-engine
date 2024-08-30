#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/logging.hpp>

using namespace ntt::event;
using namespace ntt::log;

class EventSystemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        NTT_APP_CONFIG(LogLevel::INFO, LOGGER_NONE);
    }
};

TEST_F(EventSystemTest, NormalWorkflow)
{
    u8 callTimes = 0;
    EventCallback callback = [&callTimes](ntt::event::EventCode code,
                                          void *data,
                                          const ntt::event::EventContext &context)
    {
        callTimes++;
    };

    auto id = RegisterEvent(EventCode::TEST, callback);

    TriggerEvent(EventCode::TEST, nullptr, {});

    EXPECT_EQ(callTimes, 1);

    UnregisterEvent(id);

    TriggerEvent(EventCode::TEST, nullptr, {});

    EXPECT_EQ(callTimes, 1);
}