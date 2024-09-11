#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/dev/store.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/logging.hpp>

using namespace ntt;
using namespace ntt::log;
using namespace ntt::dev::store;

struct TestObj
{
    u32 data;
    String name;

    TestObj(u32 data, const String &name) : data(data), name(name) {}
};

class StoreTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Setup code
    }

    Store<u32, TestObj> store{0, 10,
                              [](Ref<TestObj> obj, Ref<TestObj> other) -> b8
                              { return obj->data == other->data; }};
};

TEST_F(StoreTest, HappyPath)
{
    auto obj1 = CREATE_REF(TestObj, 1, "obj1");
    auto obj2 = CREATE_REF(TestObj, 3, "obj2");
    auto obj3 = CREATE_REF(TestObj, 3, "obj3");
    auto obj4 = CREATE_REF(TestObj, 4, "obj4");
    auto obj5 = CREATE_REF(TestObj, 5, "obj5");
    auto obj6 = CREATE_REF(TestObj, 6, "obj6");

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

    EXPECT_EQ(store.Get(8) == nullptr, true);

    store.Release(id3);

    EXPECT_EQ(store.Get(id3) == nullptr, true);

    EXPECT_TRUE(store.GetAvailableIds() == List<u32>({0, 2, 3}));

    auto id6 = store.Add(obj6);
    EXPECT_EQ(id6, 1);

    store.Release(id1);
    store.Release(id4);
    store.Release(23);

    EXPECT_TRUE(store.GetAvailableIds() == List<u32>({1, 3}));

    EXPECT_EQ(store.Add(CREATE_REF(TestObj, 1, "obj1")), 0);

    EXPECT_TRUE(store.GetAvailableIds() == List<u32>({0, 1, 3}));

    EXPECT_EQ(store.Add(CREATE_REF(TestObj, 44, "obj1")), 2);
}

TEST_F(StoreTest, TestForEach)
{
    auto id1 = store.Add(CREATE_REF(TestObj, 1, "obj1"));
    auto id2 = store.Add(CREATE_REF(TestObj, 3, "obj2"));
    auto id3 = store.Add(CREATE_REF(TestObj, 3, "obj3"));
    auto id4 = store.Add(CREATE_REF(TestObj, 4, "obj4"));
    auto id5 = store.Add(CREATE_REF(TestObj, 5, "obj5"));
    auto id6 = store.Add(CREATE_REF(TestObj, 6, "obj6"));

    String result = "";

    store.ForEach([&result](Ref<TestObj> obj, const u32 id)
                  { result += obj->name + "; "; });

    EXPECT_EQ(result, "obj1; obj2; obj4; obj5; obj6; ");

    store.Release(id3);

    result = "";

    store.ForEach([&result](Ref<TestObj> obj, const u32 id)
                  { result += obj->name + "; "; });

    EXPECT_EQ(result, "obj1; obj4; obj5; obj6; ");
}

TEST_F(StoreTest, MaxCondition)
{
    store.Add(CREATE_REF(TestObj, 1, "obj1"));
    store.Add(CREATE_REF(TestObj, 2, "obj2"));
    store.Add(CREATE_REF(TestObj, 3, "obj3"));
    store.Add(CREATE_REF(TestObj, 4, "obj4"));
    store.Add(CREATE_REF(TestObj, 5, "obj5"));
    store.Add(CREATE_REF(TestObj, 6, "obj6"));
    store.Add(CREATE_REF(TestObj, 7, "obj7"));
    store.Add(CREATE_REF(TestObj, 8, "obj8"));
    store.Add(CREATE_REF(TestObj, 9, "obj9"));
    store.Add(CREATE_REF(TestObj, 10, "obj10"));
    EXPECT_THAT(store.Add(CREATE_REF(TestObj, 11, "obj11")), 0);
}