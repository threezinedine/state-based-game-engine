#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/exception.hpp>

using namespace ntt;
using namespace ntt::log;
using namespace ntt::exception;

#define EXPECT_LIST_EQ(lst1, lst2)                      \
    EXPECT_TRUE(lst1.Equals(lst2))                      \
        << "Expected: " << std::string(lst2.ToString()) \
        << " but got: " << std::string(lst1.ToString())

class TestObject
{
public:
    TestObject(u8 id) : Id(id) {}

    bool Equals(const TestObject &other) const
    {
        return Id == other.Id;
    }

private:
    u8 Id;
};

class ComparableObject
{
public:
    ComparableObject(u8 id) : Id(id) {}

    bool operator==(const ComparableObject &other) const
    {
        return Id == other.Id;
    }

private:
    u8 Id;
};

class ListTest : public testing::Test
{
protected:
    void SetUp() override
    {
        NTT_ENGINE_CONFIG(LogLevel::FATAL, LOGGER_NONE);
    }
};

TEST_F(ListTest, SubListTest)
{
    List<int> lst = {1, 2, 3, 4, 5};

    EXPECT_STREQ(lst.ToString(), "[1, 2, 3, 4, 5]");

    auto subList = lst.SubList(1, 3);
    EXPECT_LIST_EQ(subList, List({2, 3}));
    EXPECT_THAT(subList.Get(0), testing::Eq(2));
    EXPECT_THAT(subList.Get(1), testing::Eq(3));
    EXPECT_THAT(subList[0], testing::Eq(2));

    EXPECT_THROW(subList.Get(2), IndexOutOfRange);
    EXPECT_THROW(subList.Get(3), IndexOutOfRange);
    EXPECT_THROW(subList[3], IndexOutOfRange);

    EXPECT_THAT(subList.Get(-1), testing::Eq(3));
    EXPECT_THAT(subList[-1], testing::Eq(3));
    EXPECT_THAT(subList.Get(-2), testing::Eq(2));
    EXPECT_THROW(subList.Get(-3), IndexOutOfRange);
    EXPECT_THROW(subList.Get(-4), IndexOutOfRange);

    EXPECT_LIST_EQ(lst.SubList(2, 1), List<i32>({}));
    EXPECT_LIST_EQ(lst.SubList(1, 1), List<i32>({}));

    EXPECT_LIST_EQ(lst.SubList(-3, -1), List({3, 4}));
    EXPECT_LIST_EQ(lst.SubList(-3, 3), List({3}));

    EXPECT_LIST_EQ(lst.SubList(0, 7), lst);
    EXPECT_LIST_EQ(lst.SubList(-7, 3), List({1, 2, 3}));

    EXPECT_LIST_EQ(lst.SubList(0, 0), List<i32>({}));
    EXPECT_LIST_EQ(lst.SubList(-1, -1), List<i32>({}));
}

TEST_F(ListTest, TestWithPremitiveData)
{
    List<int> lst = {1, 2, 3, 4, 5};

    lst.Add(6);
    lst.Add(7);
    EXPECT_LIST_EQ(lst, List({1, 2, 3, 4, 5, 6, 7}));

    lst.Remove(2);
    EXPECT_LIST_EQ(lst, List({1, 2, 4, 5, 6, 7}));
    lst.Remove(0);
    EXPECT_LIST_EQ(lst, List({2, 4, 5, 6, 7}));
    lst.Remove(-1);
    EXPECT_LIST_EQ(lst, List({2, 4, 5, 6}));

    lst.Remove(2);
    EXPECT_LIST_EQ(lst, List({2, 4, 6}));

    lst.Remove(-4);
    EXPECT_LIST_EQ(lst, List({2, 4, 6}));
    lst.Remove(-2);
    EXPECT_LIST_EQ(lst, List({2, 6}));

    lst = {1, 2, 3, 4, 5};
    lst.RemoveItem(3);
    EXPECT_LIST_EQ(lst, List({1, 2, 4, 5}));

    lst.RemoveItem(1);
    EXPECT_LIST_EQ(lst, List({2, 4, 5}));

    lst.RemoveItem(5);
    EXPECT_LIST_EQ(lst, List({2, 4}));

    lst.RemoveItem(6);
    EXPECT_LIST_EQ(lst, List({2, 4}));
}

TEST_F(ListTest, WithOtherObject)
{
    List<TestObject> lst;
    TestObject obj1(1);
    TestObject obj2(2);
    TestObject obj3(3);

    lst.Add(obj1);
    lst.Add(obj2);
    lst.Add(obj3);

    EXPECT_TRUE(lst.Length() == 3);

    auto compareCallback = [](const TestObject &a, const TestObject &b)
    { return a.Equals(b); };

    lst.RemoveItem(obj2, compareCallback);

    EXPECT_TRUE(lst.Equals({obj1, obj3}, compareCallback));
}

TEST_F(ListTest, CheckRemoveItemWithComparableObject)
{
    List<ComparableObject> lst;
    ComparableObject obj1(1);
    ComparableObject obj2(2);
    ComparableObject obj3(3);

    lst.Add(obj1);
    lst.Add(obj2);
    lst.Add(obj3);

    lst.RemoveItem(obj2);
    EXPECT_TRUE(lst.Equals({obj1, obj3}));
}

TEST_F(ListTest, TestReducing)
{
    List<u32> lst = {1, 2, 3, 4, 5};

    u32 sum = 0;
    lst.Reduce<u32>([](u32 &sum, const u32 &element, ...)
                    { sum += element; }, sum);
    EXPECT_THAT(sum, testing::Eq(15));

    u32 factorial = 1;
    lst.Reduce<u32>([](u32 &fac, const u32 &element, ...)
                    { fac *= element; }, factorial);
    EXPECT_THAT(factorial, testing::Eq(120));

    List<u32> lst2 = {2, 4, 6, 8};
    EXPECT_TRUE(lst2.All([](const u32 &element, ...)
                         { return element % 2 == 0; }));
    EXPECT_FALSE(lst2.Any([](const u32 &element, ...)
                          { return element % 2 != 0; }));

    sum = 0;
    lst2.ForEach([&sum](const u32 &element, ...)
                 { sum += element; });

    EXPECT_THAT(sum, testing::Eq(20));

    auto dbLst = lst2.Map<f32>([](const u32 &element, ...)
                               { return element * 2.0; });

    EXPECT_LIST_EQ(dbLst, List<f32>({4.0, 8.0, 12.0, 16.0}));
}