#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/logging.hpp>

using namespace ntt;
using namespace ntt::log;

#define EXPECT_STR_EQ(str1, str2)           \
    EXPECT_TRUE(str1 == str2)               \
        << "Expected: " << str2.RawString() \
        << " but got: " << str1.RawString()

class StringTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        NTT_ENGINE_CONFIG(LogLevel::FATAL, LOGGER_NONE);
    }
};

TEST_F(StringTest, TestToString)
{
    String str{"Hello, World!"};
    EXPECT_THAT(str.RawString(), testing::StrEq("Hello, World!"));

    String str2("Number of ships: %d", 10);
    EXPECT_STR_EQ(str2, String("Number of ships: 10"));

    EXPECT_STR_EQ(String("Testing").SubString(0, 2), String("Te"));

    EXPECT_THAT(String("Testing").FindFirst("ing"), testing::Eq(4));
    EXPECT_THAT(String("Testing").FindFirst("ang"), testing::Eq(String::NotFound));
    EXPECT_THAT(String("Testing").FindFirst(""), testing::Eq(0));

    String str3("Wokong, Hobong");
    str3.Replace("ong", "ing");
    EXPECT_THAT(str3.RawString(),
                testing::StrEq("Woking, Hobing"));
}

TEST_F(StringTest, StringConcat)
{
    String str = "Hello, World!";
    str.Concat("Universe");
    EXPECT_STR_EQ(str, String("Hello, World!Universe"));

    String str2 = "Hello";
    str2.Concat("World", "Hello, {src} {str}");
    EXPECT_STR_EQ(str2, String("Hello, Hello World"));

    String str3{"Hello"};
    str3.Concat("World", "Hello, {str}");
    EXPECT_STR_EQ(str3, String("Hello, World"));

    String hello = "Hello World";
    hello += ", Test";
    EXPECT_STR_EQ(hello, String("Hello World, Test"));

    String hello2 = hello + ", Test2";
    EXPECT_STR_EQ(hello2, String("Hello World, Test, Test2"));
    EXPECT_STR_EQ(hello, String("Hello World, Test"));

    String spacesStr = "   Hello, World!   ";
    spacesStr.Trim();
    EXPECT_STR_EQ(spacesStr, String("Hello, World!"));

    String splitTestStr = "Hello, World, Test";
    auto splittedStr = splitTestStr.Split(", ");
    EXPECT_TRUE(splittedStr.Equals(List<String>{"Hello", "World", "Test"}));
    EXPECT_TRUE(String(", Testing, Method").Split(", ").Equals(List<String>{"", "Testing", "Method"}));

    EXPECT_TRUE(splitTestStr.Split(": ").Equals(List<String>{"Hello, World, Test"}));
}

TEST_F(StringTest, StringPatternMatching)
{
    String str = "Hello, World first";
    EXPECT_TRUE(str.MatchPattern("Hello, {} first"));
    EXPECT_FALSE(str.MatchPattern("Hello, {} second"));
    EXPECT_FALSE(str.MatchPattern("{}, {} second"));
    EXPECT_TRUE(str.MatchPattern("{}, {} first"));
    EXPECT_TRUE(str.MatchPattern("{}, {}World{} first"));
    EXPECT_TRUE(str.MatchPattern("Hello, World {}"));
}