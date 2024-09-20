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
    EventCallback callback = [&callTimes](event_code_t code,
                                          void *data,
                                          const ntt::event::EventContext &context)
    {
        callTimes++;
    };

    auto id = RegisterEvent(NTT_EVENT_TEST, callback);

    TriggerEvent(NTT_EVENT_TEST, nullptr, {});

    EXPECT_EQ(callTimes, 1);

    UnregisterEvent(id);

    TriggerEvent(NTT_EVENT_TEST, nullptr, {});

    EXPECT_EQ(callTimes, 1);
}

TEST_F(EventSystemTest, UnregisterNonExistEvent)
{
    EXPECT_NO_THROW(UnregisterEvent(0));
}

TEST_F(EventSystemTest, TriggerNonExistEvent)
{
    EXPECT_NO_THROW(TriggerEvent(0xDD, nullptr, {}));
}