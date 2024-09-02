#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/dev/store.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/logging.hpp>
#include "../store_conf_t.hpp"

using namespace ntt;
using namespace ntt::log;
using namespace ntt::dev::store;

class StoreTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Setup code
    }

    Store<u32, TestObj, u32> store{0, 10,
                                   [](const TestObj &obj)
                                   { return obj.data; }};
};

TEST_F(StoreTest, HappyPath)
{
    auto obj1 = TestObj{1, "obj1"};
    auto obj2 = TestObj{3, "obj2"};
    auto obj3 = TestObj{3, "obj2"};
    auto obj4 = TestObj{4, "obj4"};
    auto obj5 = TestObj{5, "obj5"};
    auto obj6 = TestObj{6, "obj6"};

    auto id1 = store.Add(obj1);
    auto id2 = store.Add(obj2);
    auto id3 = store.Add(obj3);
    auto id4 = store.Add(obj4);
    auto id5 = store.Add(obj5);

    EXPECT_EQ(id1, 0);
    EXPECT_EQ(id2, 1);
    EXPECT_EQ(id3, 1);
    EXPECT_EQ(id4, 2);
    EXPECT_EQ(id5, 3);

    EXPECT_EQ(store.Get(id1)->data, 1);
    EXPECT_EQ(store.Get(id1)->name, "obj1");
    EXPECT_EQ(store.Get(id2)->data, 3);
    EXPECT_EQ(store.Get(id2)->name, "obj2");
    EXPECT_EQ(store.Get(id3)->data, 3);
    EXPECT_EQ(store.Get(id3)->name, "obj2");

    store.Release(id3);

    EXPECT_TRUE(store.GetAvailableIds() == List<u32>({0, 2, 3}));

    auto id6 = store.Add(obj6);
    EXPECT_EQ(id6, 1);

    store.Release(id1);
    store.Release(id4);
    store.Release(23);

    EXPECT_TRUE(store.GetAvailableIds() == List<u32>({1, 3}));

    EXPECT_EQ(store.Add({1, "obj1"}), 0);

    EXPECT_TRUE(store.GetAvailableIds() == List<u32>({0, 1, 3}));
}