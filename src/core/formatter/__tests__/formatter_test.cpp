#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/core/formatter.hpp>

using namespace ntt;

TEST(Formatter, FormatString)
{
    EXPECT_EQ(format("{}, World", std::string("Hello")), "Hello, World");
    EXPECT_EQ(format("{}, World", "Hello"), "Hello, World");
    EXPECT_EQ(format("Hello, {}", String("World")), "Hello, World");
    EXPECT_EQ(format("Hello, {}", "World"), "Hello, World");
}