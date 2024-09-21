#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/structures/sorted_list.hpp>

using namespace ntt;

class SortedListTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(SortedListTest, AddAndRemoveItem)
{
    SortedList<u32> list;
    list.Add(2);
    list.Add(1);
    list.Add(3);
    list.Add(5);
    list.Add(4);
    list.Add(6);

    EXPECT_EQ(list, SortedList<u32>({1, 2, 3, 4, 5, 6}));
}

TEST_F(SortedListTest, TestAddingWithComapator)
{
    struct Data
    {
        u32 id;
        std::string name;
    };

    SortedList<Data> list([](const Data &a, const Data &b)
                          { return a.id - b.id; });

    list.Add({2, "two"});
    list.Add({1, "one"});
    list.Add({3, "three"});
    list.Add({5, "five"});

    EXPECT_EQ(list, SortedList<Data>({{1, "one"}, {2, "two"}, {3, "three"}, {5, "five"}}));
}