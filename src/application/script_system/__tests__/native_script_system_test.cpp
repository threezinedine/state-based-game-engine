#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/script_system/script_system.hpp>
#include <NTTEngine/application/script_system/native_script_system.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/profiling.hpp>

using namespace ntt;
using namespace ntt::script;
using namespace ntt::event;
using namespace ntt::ecs;

namespace
{
    u8 s_enterCalled = 0;
    u8 s_updateCalled = 0;
    u8 s_exitCalled = 0;
} // namespace

class TestController : public Script
{
protected:
    void OnEnterImpl() override
    {
        s_enterCalled++;
    }

    void OnExitImpl() override
    {
        s_exitCalled++;
    }

    void OnUpdateImpl(f32 deltaTime) override
    {
        s_updateCalled++;
    }
};

class NativeScriptSystemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        s_enterCalled = 0;
        s_updateCalled = 0;
        s_exitCalled = 0;

        EventInit();
        ECSInit();

        ECSRegister(
            "Native Script System",
            CreateRef<ScriptSystem>(),
            {typeid(NativeScriptComponent)});
    }

    void TearDown() override
    {
        ECSShutdown();
        EventShutdown();
    }
};

TEST_F(NativeScriptSystemTest, NormalWorkflow)
{
    auto entity = ECSCreateEntity(
        "test",
        {ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<TestController>())});

    ECSUpdate(0.1f);
    EXPECT_EQ(ECS_GET_COMPONENT(entity, NativeScriptComponent)->ins->GetEntity(), entity);
    EXPECT_EQ(s_enterCalled, 1);
    EXPECT_EQ(s_updateCalled, 1);

    ECSUpdate(0.1f);
    EXPECT_EQ(s_enterCalled, 1);
    EXPECT_EQ(s_updateCalled, 2);
    EXPECT_EQ(s_exitCalled, 0);

    ECSDeleteEntity(entity);
    EXPECT_EQ(s_exitCalled, 1);

    ECSUpdate(0.1f);
    EXPECT_EQ(s_enterCalled, 1);
    EXPECT_EQ(s_updateCalled, 2);
    EXPECT_EQ(s_exitCalled, 1);
}

TEST_F(NativeScriptSystemTest, NoScriptComponent)
{
    auto entity = ECSCreateEntity("test", {});

    ECSUpdate(0.1f);
    EXPECT_EQ(s_enterCalled, 0);
    EXPECT_EQ(s_updateCalled, 0);
    EXPECT_EQ(s_exitCalled, 0);

    ECSDeleteEntity(entity);
    EXPECT_EQ(s_exitCalled, 0);
}

TEST_F(NativeScriptSystemTest, EmptyScript)
{
    auto entity = ECSCreateEntity(
        "test",
        {ECS_CREATE_COMPONENT(NativeScriptComponent, nullptr)});

    ECSUpdate(0.1f);
    EXPECT_EQ(s_enterCalled, 0);
    EXPECT_EQ(s_updateCalled, 0);
    EXPECT_EQ(s_exitCalled, 0);

    ECSDeleteEntity(entity);
    EXPECT_EQ(s_exitCalled, 0);
}