#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/script_system/native_system.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/core/logging.hpp>

using namespace ntt;
using namespace ntt::script;
using namespace ntt::event;
using namespace ntt::memory;
using namespace ntt::log;

namespace
{
} // namespace

class TestScript;

#define TEST_EVENT 0

namespace
{
    u8 s_testEventCalled = 0;
} // namespace

class ScriptableTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        s_testEventCalled = 0;

        LogInit();
        ConfigureLogger(ENGINE_LOGGER_NAME, LogLevel::INFO, LOGGER_NONE);
        ProfilingInit("profiling", TRUE);
        EventInit();
    }

    void TearDown() override
    {
        EventShutdown();
        ProfilingShutdown();
    }

    Scope<TestScript> script;
};

class TestScript : public Scriptable
{
public:
    void OnEnter() override
    {
        Subscribe(TEST_EVENT, [this](...)
                  { s_testEventCalled++; });
    }

    void OnExit() override
    {
        Delete();
    }
};

TEST_F(ScriptableTest, TestEventSubscribe)
{
    script = CreateScope<TestScript>();

    script->OnEnter();

    TriggerEvent(TEST_EVENT);
    EXPECT_EQ(s_testEventCalled, 1);

    TriggerEvent(TEST_EVENT);
    EXPECT_EQ(s_testEventCalled, 2);

    script->OnExit();

    TriggerEvent(TEST_EVENT);
    EXPECT_EQ(s_testEventCalled, 2);
}