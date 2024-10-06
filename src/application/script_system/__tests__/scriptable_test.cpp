#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/script_system/native_script.hpp>
#include <NTTEngine/application/script_system/script_system.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/profiling.hpp>

using namespace ntt;
using namespace ntt::script;
using namespace ntt::event;
using namespace ntt::memory;

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
        Subscribe(TEST_EVENT);
    }

    void OnExit() override
    {
        Delete();
    }

protected:
    void OnEvent(event_code_t code, void *sender, const EventContext &context) override
    {
        s_testEventCalled++;
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

    TriggerEvent(TEST_EVENT);
    EXPECT_EQ(s_testEventCalled, 2);
}