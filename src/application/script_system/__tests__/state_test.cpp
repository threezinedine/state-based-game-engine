#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/script_system/state.hpp>
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

    u8 s_state1_child1_OnEnter = 0;
    u8 s_state1_child1_OnExit = 0;
    u8 s_state1_child1_OnUpdate = 0;

    u8 s_state1_child2_OnEnter = 0;
    u8 s_state1_child2_OnExit = 0;
    u8 s_state1_child2_OnUpdate = 0;

    b8 s_shouldMoveToState2 = FALSE;
    b8 s_shouldMoveToState1 = FALSE;
    b8 s_shouldMoveToStateChild1 = FALSE;
    b8 s_shouldMoveToStateChild2 = FALSE;
}

#define TEST_STATE_1 "TestState1"
#define TEST_STATE_CHILD1 "TestStateChild1"
#define TEST_STATE_CHILD2 "TestStateChild2"

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

class TestStateChild1 : public State
{
protected:
    void OnEnterImpl() override
    {
        s_state1_child1_OnEnter++;
    }

    String OnNavigateImpl() override
    {
        if (s_shouldMoveToStateChild2)
        {
            return TEST_STATE_CHILD2;
        }
        return KEEP_STATE;
    }

    void OnUpdateImpl(f32 delta) override
    {
        s_state1_child1_OnUpdate++;
    }

    void OnExitImpl() override
    {
        s_state1_child1_OnExit++;
    }
};

class TestStateChild2 : public State
{
protected:
    void OnEnterImpl() override
    {
        s_state1_child2_OnEnter++;
    }

    String OnNavigateImpl() override
    {
        if (s_shouldMoveToStateChild1)
        {
            return TEST_STATE_CHILD1;
        }
        return KEEP_STATE;
    }

    void OnUpdateImpl(f32 delta) override
    {
        s_state1_child2_OnUpdate++;
    }

    void OnExitImpl() override
    {
        s_state1_child2_OnExit++;
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

        s_state1_child1_OnEnter = 0;
        s_state1_child1_OnExit = 0;
        s_state1_child1_OnUpdate = 0;

        s_state1_child2_OnEnter = 0;
        s_state1_child2_OnExit = 0;
        s_state1_child2_OnUpdate = 0;

        m_machine = CreateScope<State>();

        m_state1 = CreateRef<TestState>();
        m_state2 = CreateRef<TestState2>();
        m_stateChild1 = CreateRef<TestStateChild1>();
        m_stateChild2 = CreateRef<TestStateChild2>();

        m_state1->AddChild(TEST_STATE_CHILD1, m_stateChild1);
        m_state1->AddChild(TEST_STATE_CHILD2, m_stateChild2);

        m_machine->AddChild(TEST_STATE_1, m_state1);
        m_machine->AddChild(TEST_STATE_2, m_state2);

        m_machine->OnEnter();
    }

    void TearDown() override
    {
    }

    Ref<State> m_state1, m_state2, m_stateChild1, m_stateChild2;
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
    s_shouldMoveToState1 = FALSE;
    s_shouldMoveToState2 = FALSE;
    s_shouldMoveToStateChild1 = FALSE;
    s_shouldMoveToStateChild2 = FALSE;

    m_machine->OnUpdate(0.0f);

    EXPECT_EQ(s_state1_OnEnter, 1);
    EXPECT_EQ(s_state1_OnUpdate, 1);
    EXPECT_EQ(s_state1_OnExit, 0);

    EXPECT_EQ(s_state1_child1_OnEnter, 1);
    EXPECT_EQ(s_state1_child1_OnUpdate, 1);
    EXPECT_EQ(s_state1_child1_OnExit, 0);

    EXPECT_EQ(s_state1_child2_OnEnter, 0);
    EXPECT_EQ(s_state1_child2_OnUpdate, 0);
    EXPECT_EQ(s_state1_child2_OnExit, 0);

    m_machine->OnUpdate(0.0f);

    EXPECT_EQ(s_state1_OnEnter, 1);
    EXPECT_EQ(s_state1_OnUpdate, 2);
    EXPECT_EQ(s_state1_OnExit, 0);

    EXPECT_EQ(s_state1_child1_OnEnter, 1);
    EXPECT_EQ(s_state1_child1_OnUpdate, 2);
    EXPECT_EQ(s_state1_child1_OnExit, 0);

    EXPECT_EQ(s_state1_child2_OnEnter, 0);
    EXPECT_EQ(s_state1_child2_OnUpdate, 0);
    EXPECT_EQ(s_state1_child2_OnExit, 0);

    s_shouldMoveToStateChild2 = TRUE;
    m_machine->OnUpdate(0.0f);
    s_shouldMoveToStateChild2 = FALSE;

    EXPECT_EQ(s_state1_OnEnter, 1);
    EXPECT_EQ(s_state1_OnUpdate, 3);
    EXPECT_EQ(s_state1_OnExit, 0);

    EXPECT_EQ(s_state1_child1_OnEnter, 1);
    EXPECT_EQ(s_state1_child1_OnUpdate, 2);
    EXPECT_EQ(s_state1_child1_OnExit, 1);

    EXPECT_EQ(s_state1_child2_OnEnter, 1);
    EXPECT_EQ(s_state1_child2_OnUpdate, 1);
    EXPECT_EQ(s_state1_child2_OnExit, 0);

    s_shouldMoveToStateChild1 = TRUE;
    m_machine->OnUpdate(0.0f);
    s_shouldMoveToStateChild1 = FALSE;

    EXPECT_EQ(s_state1_OnEnter, 1);
    EXPECT_EQ(s_state1_OnUpdate, 4);
    EXPECT_EQ(s_state1_OnExit, 0);

    EXPECT_EQ(s_state1_child1_OnEnter, 2);
    EXPECT_EQ(s_state1_child1_OnUpdate, 3);
    EXPECT_EQ(s_state1_child1_OnExit, 1);

    EXPECT_EQ(s_state1_child2_OnEnter, 1);
    EXPECT_EQ(s_state1_child2_OnUpdate, 1);
    EXPECT_EQ(s_state1_child2_OnExit, 1);

    s_shouldMoveToState2 = TRUE;
    m_machine->OnUpdate(0.0f);
    s_shouldMoveToState2 = FALSE;

    EXPECT_EQ(s_state1_OnEnter, 1);
    EXPECT_EQ(s_state1_OnUpdate, 4);
    EXPECT_EQ(s_state1_OnExit, 1);

    EXPECT_EQ(s_state1_child1_OnEnter, 2);
    EXPECT_EQ(s_state1_child1_OnUpdate, 3);
    EXPECT_EQ(s_state1_child1_OnExit, 2);

    EXPECT_EQ(s_state1_child2_OnEnter, 1);
    EXPECT_EQ(s_state1_child2_OnUpdate, 1);
    EXPECT_EQ(s_state1_child2_OnExit, 1);

    EXPECT_EQ(s_state2_OnEnter, 1);
    EXPECT_EQ(s_state2_OnUpdate, 1);
    EXPECT_EQ(s_state2_OnExit, 0);

    s_shouldMoveToState1 = TRUE;
    m_machine->OnUpdate(0.0f);
    s_shouldMoveToState1 = FALSE;

    EXPECT_EQ(s_state1_OnEnter, 2);
    EXPECT_EQ(s_state1_OnUpdate, 5);
    EXPECT_EQ(s_state1_OnExit, 1);

    EXPECT_EQ(s_state1_child1_OnEnter, 3);
    EXPECT_EQ(s_state1_child1_OnUpdate, 4);
    EXPECT_EQ(s_state1_child1_OnExit, 2);

    EXPECT_EQ(s_state1_child2_OnEnter, 1);
    EXPECT_EQ(s_state1_child2_OnUpdate, 1);
    EXPECT_EQ(s_state1_child2_OnExit, 1);

    s_shouldMoveToStateChild2 = TRUE;
    m_machine->OnUpdate(0.0f);
    s_shouldMoveToStateChild2 = FALSE;

    EXPECT_EQ(s_state1_OnEnter, 2);
    EXPECT_EQ(s_state1_OnUpdate, 6);
    EXPECT_EQ(s_state1_OnExit, 1);

    EXPECT_EQ(s_state1_child1_OnEnter, 3);
    EXPECT_EQ(s_state1_child1_OnUpdate, 4);
    EXPECT_EQ(s_state1_child1_OnExit, 3);

    EXPECT_EQ(s_state1_child2_OnEnter, 2);
    EXPECT_EQ(s_state1_child2_OnUpdate, 2);
    EXPECT_EQ(s_state1_child2_OnExit, 1);

    s_shouldMoveToState2 = TRUE;
    m_machine->OnUpdate(0.0f);
    s_shouldMoveToState2 = FALSE;

    EXPECT_EQ(s_state1_OnEnter, 2);
    EXPECT_EQ(s_state1_OnUpdate, 6);
    EXPECT_EQ(s_state1_OnExit, 2);

    EXPECT_EQ(s_state1_child1_OnEnter, 3);
    EXPECT_EQ(s_state1_child1_OnUpdate, 4);
    EXPECT_EQ(s_state1_child1_OnExit, 3);

    EXPECT_EQ(s_state1_child2_OnEnter, 2);
    EXPECT_EQ(s_state1_child2_OnUpdate, 2);
    EXPECT_EQ(s_state1_child2_OnExit, 2);

    m_machine->OnUpdate(0.0f);

    s_shouldMoveToState1 = TRUE;
    m_machine->OnUpdate(0.0f);
    s_shouldMoveToState1 = FALSE;

    EXPECT_EQ(s_state1_OnEnter, 3);
    EXPECT_EQ(s_state1_OnUpdate, 7);
    EXPECT_EQ(s_state1_OnExit, 2);

    EXPECT_EQ(s_state1_child1_OnEnter, 4);
    EXPECT_EQ(s_state1_child1_OnUpdate, 5);
    EXPECT_EQ(s_state1_child1_OnExit, 3);

    EXPECT_EQ(s_state1_child2_OnEnter, 2);
    EXPECT_EQ(s_state1_child2_OnUpdate, 2);
    EXPECT_EQ(s_state1_child2_OnExit, 2);
}
