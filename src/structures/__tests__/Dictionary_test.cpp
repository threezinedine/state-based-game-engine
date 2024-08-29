#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/logging.hpp>

using namespace ntt;
using namespace ntt::log;

class DictionaryTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        NTT_ENGINE_CONFIG(LogLevel::FATAL, LOGGER_NONE);
    }
};

TEST_F(DictionaryTest, TestAssignment)
{
    Dictionary<String, u32> dict;

    dict.Insert("key1", 1);
    EXPECT_EQ(dict.Get("key1"), 1);

    dict["key2"] = 2;
    EXPECT_EQ(dict["key2"], 2);

    Dictionary<String, String> dict2 = {
        {"key1", "value1"},
        {"key2", "value2"}};

    EXPECT_TRUE(dict2["key1"] == "value1");
    EXPECT_TRUE(dict2["key2"] == "value2");

    EXPECT_TRUE(dict2.Contains("key1"));
    EXPECT_TRUE(dict2.Contains("key2"));
    EXPECT_FALSE(dict2.Contains("key3"));
}

TEST_F(DictionaryTest, TestForEach)
{
    Dictionary<String, u32> dict = {
        {"key1", 1},
        {"key2", 2},
        {"key3", 3}};

    u32 sum = 0;
    dict.ForEach([&sum](const String &key, u32 &value)
                 { sum += value; });

    EXPECT_EQ(sum, 6);
}

TEST_F(DictionaryTest, TestReduce)
{
    Dictionary<String, u32> dict = {
        {"key1", 1},
        {"test_key2", 2},
        {"key3", 3}};

    Dictionary<String, u32> testDict;
    dict.Reduce<Dictionary<String, u32>>(
        [](const String &key, u32 &value, Dictionary<String, u32> &dict)
        { 
                    if (key.FindFirst("key") != String::NotFound)
                    {
                        dict[key] = value;
                    } },
        testDict);

    EXPECT_THAT(testDict["test_key2"], 2);
}