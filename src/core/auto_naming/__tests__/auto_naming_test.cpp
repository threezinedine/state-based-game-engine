#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/core/auto_naming.hpp>
#include <NTTEngine/core/formatter.hpp>

using namespace ntt;

#define EXPECT_STR_EQ(a, b)                                             \
    EXPECT_EQ(a, b);                                                    \
    if (a != b)                                                         \
    {                                                                   \
        std::cout << format("{} != {}", a, b).RawString() << std::endl; \
    }

class NamingModuleTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        NamingSystemInit();
    }

    void TearDown() override
    {
        NamingSystemShutdown();
    }
};

TEST_F(NamingModuleTest, GetNameTest)
{
    auto name = GetName("Player");
    auto name2 = GetName("Player ");
    auto name3 = GetName("Player");
    auto name4 = GetName("Player (4)");
    auto name5 = GetName("Player");

    EXPECT_STR_EQ(name, "Player");
    EXPECT_STR_EQ(name2, "Player (1)");
    EXPECT_STR_EQ(name3, "Player (2)");
    EXPECT_STR_EQ(name4, "Player (3)");
    EXPECT_STR_EQ(name5, "Player (4)");

    auto testing = GetName("Testing");

    EXPECT_STR_EQ(testing, "Testing");
}

TEST_F(NamingModuleTest, NamingSystemResetTest)
{
    auto name = GetName("Player");
    auto name2 = GetName("Player ");    /// The name2 will be "Player (1)"
                                        /// the space will be irgnored
    auto name3 = GetName("Player");     /// The name3 will be "Player (2)"
    auto name4 = GetName("Player (4)"); /// The name4 will be "Player (3)"

    NamingSystemReset();
    auto name5 = GetName("Player (4)"); /// The name5 will be "Player"
    auto name6 = GetName("Player");     /// The name5 will be "Player (1)"

    EXPECT_STR_EQ(name5, "Player");
    EXPECT_STR_EQ(name6, "Player (1)");
}