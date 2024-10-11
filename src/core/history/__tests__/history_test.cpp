#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/history/history.hpp>

using namespace ntt;
using namespace ntt::event;

static b8 canExecute = TRUE;
static u8 executeCount = 0;
static u8 undoCount = 0;

static b8 historyEmpty = TRUE;

class TestAction : public Action
{
public:
    b8 CanExecute() override
    {
        return canExecute;
    }

    void Execute() override
    {
        executeCount++;
    }

    void Undo() override
    {
        undoCount++;
    }
};

class HistoryTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        EventInit();
        HistoryManager_Init();
        canExecute = TRUE;
        historyEmpty = TRUE;
        executeCount = 0;
        undoCount = 0;

        RegisterEvent(NTT_HISTORY_EMPTY,
                      std::bind(
                          &HistoryTest::OnHistoryEmpty,
                          this,
                          std::placeholders::_1,
                          std::placeholders::_2,
                          std::placeholders::_3));
        RegisterEvent(NTT_HISTORY_NOT_EMPTY,
                      std::bind(
                          &HistoryTest::OnHistoryNotEmpty,
                          this,
                          std::placeholders::_1,
                          std::placeholders::_2,
                          std::placeholders::_3));
    }

    void TearDown() override
    {
        HistoryManager_Shutdown();
        EventShutdown();
    }

    void OnHistoryEmpty(event_code_t code, void *sender, const EventContext &context)
    {
        historyEmpty = TRUE;
    }

    void OnHistoryNotEmpty(event_code_t code, void *sender, const EventContext &context)
    {
        historyEmpty = FALSE;
    }
};

TEST_F(HistoryTest, AddExecutableAction)
{
    Ref<Action> action = CreateRef<TestAction>();
    HistoryManager_AddAction(action);

    EXPECT_EQ(executeCount, 1);
    EXPECT_FALSE(historyEmpty);
    EXPECT_FALSE(HistoryManager_IsEmpty());
    EXPECT_EQ(undoCount, 0);
}

TEST_F(HistoryTest, UndoAction)
{
    Ref<Action> action = CreateRef<TestAction>();
    HistoryManager_AddAction(action);

    EXPECT_EQ(executeCount, 1);
    EXPECT_FALSE(historyEmpty);
    EXPECT_FALSE(HistoryManager_IsEmpty());
    HistoryManager_Undo();
    EXPECT_EQ(undoCount, 1);
    EXPECT_TRUE(historyEmpty);
    EXPECT_TRUE(HistoryManager_IsEmpty());
}

TEST_F(HistoryTest, UndoEmptyAction)
{
    EXPECT_NO_THROW(HistoryManager_Undo());
    EXPECT_EQ(undoCount, 0);
    EXPECT_TRUE(HistoryManager_IsEmpty());
    EXPECT_TRUE(historyEmpty);
}

TEST_F(HistoryTest, AddNonExecutableAction)
{
    canExecute = FALSE;
    Ref<Action> action = CreateRef<TestAction>();
    HistoryManager_AddAction(action);

    EXPECT_EQ(executeCount, 0);
    EXPECT_TRUE(HistoryManager_IsEmpty());
    EXPECT_TRUE(historyEmpty);
    EXPECT_EQ(undoCount, 0);
}

TEST_F(HistoryTest, ClearHistory)
{
    Ref<Action> action = CreateRef<TestAction>();
    HistoryManager_AddAction(action);

    Ref<Action> action2 = CreateRef<TestAction>();
    HistoryManager_AddAction(action2);

    EXPECT_EQ(executeCount, 2);
    EXPECT_FALSE(HistoryManager_IsEmpty());
    EXPECT_FALSE(historyEmpty);
    HistoryManager_Clear();
    EXPECT_TRUE(HistoryManager_IsEmpty());
    EXPECT_TRUE(historyEmpty);
    EXPECT_EQ(undoCount, 0);
}