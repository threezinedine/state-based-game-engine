#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <nlohmann/json.hpp>
#include <NTTEngine/core/parser/json.hpp>

using namespace ntt;

class JSONTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Setup code
        json = R"({
            "name": "John Doe",
            "age": 30,
            "active": true,
            "speed": 34.3,
            "entity": {
                "name": "Entity",
                "scores": [9, 8, 9]
            },
            "scenes": [
                {
                    "name": "Scene 1",
                    "active": true
                },
                {
                    "name": "Scene 2",
                    "active": false
                }
            ],
            "list": [1, 2, 3]
        })";
    }

    JSON json;
};

TEST_F(JSONTest, TestContainsMethod)
{
    EXPECT_EQ(json.Contains<String>("name"), TRUE);
    EXPECT_EQ(json.Contains<i32>("name"), FALSE);
    EXPECT_EQ(json.Contains<u32>("name"), FALSE);
    EXPECT_EQ(json.Contains<b8>("name"), FALSE);
    EXPECT_EQ(json.Contains<f32>("name"), FALSE);

    EXPECT_EQ(json.Contains<u32>("age"), TRUE);
    EXPECT_EQ(json.Contains<i32>("age"), TRUE);
    EXPECT_EQ(json.Contains<f32>("age"), TRUE);
    EXPECT_EQ(json.Contains<String>("age"), FALSE);

    EXPECT_EQ(json.Contains<b8>("active"), TRUE);
    EXPECT_EQ(json.Contains<b32>("active"), TRUE);
    EXPECT_EQ(json.Contains<i32>("active"), FALSE);
    EXPECT_EQ(json.Contains<u32>("active"), TRUE);
    EXPECT_EQ(json.Contains<f32>("active"), FALSE);

    EXPECT_EQ(json.Contains<f32>("speed"), TRUE);

    EXPECT_EQ(json.ContainsList<f32>("list"), TRUE);
    EXPECT_EQ(json.ContainsList<String>("list"), FALSE);

    EXPECT_EQ(json.ContainsList<String>("name"), FALSE);
    EXPECT_EQ(json.Contains<String>("unknown"), FALSE);

    EXPECT_EQ(json.Contains<JSON>("entity"), TRUE);
    EXPECT_EQ(json.Contains<JSON>("name"), FALSE);
    EXPECT_EQ(json.Contains<f32>("entity"), FALSE);
}

TEST_F(JSONTest, TestGetDataFeature)
{
    EXPECT_EQ(json.Get<String>("name", "Unknown"), "John Doe");
    EXPECT_EQ(json.Get<i32>("age", 0), 30);
    EXPECT_EQ(json.Get<u32>("age", 0), 30);
    EXPECT_EQ(json.Get<f32>("age", 0.0f), 30.0f);
    EXPECT_EQ(json.Get<f32>("speed", 0.0f), 34.3f);
    EXPECT_EQ(json.Get<u32>("speed", 0.0f), 34);

    EXPECT_EQ(json.Get<String>("speed", "Unknown"), "Unknown");
    EXPECT_EQ(json.Get<String>("unknown", "Unknown"), "Unknown");
    EXPECT_EQ(json.Get<i32>("unknown", -2), -2);

    EXPECT_EQ(json.Get<b8>("active", FALSE), TRUE);
    EXPECT_EQ(json.Get<b32>("active", FALSE), TRUE);

    EXPECT_EQ(json.Get<f32>("active", 0.0f), 0.0f);

    auto entity = json.Get<JSON>("entity");
    EXPECT_EQ(entity.Get<String>("name", "Unknown"), "Entity");
    EXPECT_EQ(entity.GetList<u32>("scores"), List<u32>({9, 8, 9}));
}

TEST_F(JSONTest, TestGetList)
{
    auto list = json.GetList<u32>("list");

    EXPECT_EQ(list, List<u32>({1, 2, 3}));
    EXPECT_EQ(json.GetList<u32>("name"), List<u32>());

    auto list2 = json.GetList<f32>("list");
    EXPECT_EQ(list2, List<f32>({1.0f, 2.0f, 3.0f}));

    auto scenes = json.GetList<JSON>("scenes");
    EXPECT_EQ(scenes.Length(), 2);
    EXPECT_EQ(scenes[0].Get<String>("name", "Unknown"), "Scene 1");
}