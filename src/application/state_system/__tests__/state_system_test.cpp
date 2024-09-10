#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/state_system/state_system.hpp>
#include <NTTEngine/core/memory.hpp>
using namespace ntt;
using namespace ntt::memory;

namespace
{
    u8 s_state1_OnEnter = 0;
    u8 s_state1_OnExit = 0;
    u8 s_state1_OnUpdate = 0;

    u8 s_state2_OnEnter = 0;
    u8 s_state2_OnExit = 0;
    u8 s_state2_OnUpdate = 0;

    b8 s_shouldMoveToState2 = FALSE;
    b8 s_shouldMoveToState1 = FALSE;
}

#define TEST_STATE_1 "TestState1"
#define TEST_STATE_2 "TestState2"

class TestState : public State
{
protected:
    void OnEnterImpl() override
    {
        s_state1_OnEnter++;
    }

    String OnNavigateImpl() override
    {
        if (s_shouldMoveToState2)
        {
            return TEST_STATE_2;
        }
        return KEEP_STATE;
    }

    void OnUpdateImpl(f32 delta) override
    {
        s_state1_OnUpdate++;
    }

    void OnExitImpl() override
    {
        s_state1_OnExit++;
    }
};

class TestState2 : public State
{
protected:
    void OnEnterImpl() override
    {
        s_state2_OnEnter++;
    }

    String OnNavigateImpl() override
    {
        if (s_shouldMoveToState1)
        {
            return TEST_STATE_1;
        }
        return KEEP_STATE;
    }

    void OnUpdateImpl(f32 delta) override
    {
        s_state2_OnUpdate++;
    }

    void OnExitImpl() override
    {
        s_state2_OnExit++;
    }
};

class StateSystemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        s_state1_OnEnter = 0;
        s_state1_OnExit = 0;
        s_state1_OnUpdate = 0;

        s_state2_OnEnter = 0;
        s_state2_OnExit = 0;
        s_state2_OnUpdate = 0;
        StateInit();

        m_machine = CreateScope<State>();

        m_machine->AddChild(TEST_STATE_1, CreateRef<TestState>());
        m_machine->AddChild(TEST_STATE_2, CreateRef<TestState2>());

        m_machine->OnEnter();
    }

    void TearDown() override
    {
        StateShutdown();
    }

    Scope<State> m_machine;
};

TEST_F(StateSystemTest, TestNormalPath)
{
    s_shouldMoveToState1 = FALSE;
    s_shouldMoveToState2 = FALSE;
    m_machine->OnUpdate(0.0f);
    EXPECT_EQ(s_state1_OnEnter, 1);
    EXPECT_EQ(s_state1_OnUpdate, 1);
    EXPECT_EQ(s_state1_OnExit, 0);

    EXPECT_EQ(s_state2_OnEnter, 0);
    EXPECT_EQ(s_state2_OnUpdate, 0);
    EXPECT_EQ(s_state2_OnExit, 0);

    m_machine->OnUpdate(0.0f);
    EXPECT_EQ(s_state1_OnEnter, 1);
    EXPECT_EQ(s_state1_OnUpdate, 2);
    EXPECT_EQ(s_state1_OnExit, 0);

    EXPECT_EQ(s_state2_OnEnter, 0);
    EXPECT_EQ(s_state2_OnUpdate, 0);
    EXPECT_EQ(s_state2_OnExit, 0);

    m_machine->OnUpdate(0.0f);
    EXPECT_EQ(s_state1_OnEnter, 1);
    EXPECT_EQ(s_state1_OnUpdate, 3);
    EXPECT_EQ(s_state1_OnExit, 0);

    EXPECT_EQ(s_state2_OnEnter, 0);
    EXPECT_EQ(s_state2_OnUpdate, 0);
    EXPECT_EQ(s_state2_OnExit, 0);

    s_shouldMoveToState1 = FALSE;
    s_shouldMoveToState2 = TRUE;
    m_machine->OnUpdate(0.0f);

    EXPECT_EQ(s_state1_OnEnter, 1);
    EXPECT_EQ(s_state1_OnUpdate, 3);
    EXPECT_EQ(s_state1_OnExit, 1);

    EXPECT_EQ(s_state2_OnEnter, 1);
    EXPECT_EQ(s_state2_OnUpdate, 1);
    EXPECT_EQ(s_state2_OnExit, 0);

    m_machine->OnUpdate(0.0f);
    EXPECT_EQ(s_state1_OnEnter, 1);
    EXPECT_EQ(s_state1_OnUpdate, 3);
    EXPECT_EQ(s_state1_OnExit, 1);

    EXPECT_EQ(s_state2_OnEnter, 1);
    EXPECT_EQ(s_state2_OnUpdate, 2);
    EXPECT_EQ(s_state2_OnExit, 0);

    s_shouldMoveToState2 = FALSE;

    m_machine->OnUpdate(0.0f);
    EXPECT_EQ(s_state1_OnEnter, 1);
    EXPECT_EQ(s_state1_OnUpdate, 3);
    EXPECT_EQ(s_state1_OnExit, 1);

    EXPECT_EQ(s_state2_OnEnter, 1);
    EXPECT_EQ(s_state2_OnUpdate, 3);
    EXPECT_EQ(s_state2_OnExit, 0);

    s_shouldMoveToState1 = TRUE;
    m_machine->OnUpdate(0.0f);

    EXPECT_EQ(s_state1_OnEnter, 2);
    EXPECT_EQ(s_state1_OnUpdate, 4);
    EXPECT_EQ(s_state1_OnExit, 1);

    EXPECT_EQ(s_state2_OnEnter, 1);
    EXPECT_EQ(s_state2_OnUpdate, 3);
    EXPECT_EQ(s_state2_OnExit, 1);
}

TEST_F(StateSystemTest, TestHierachyStates)
{
}