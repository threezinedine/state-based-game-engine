#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/resources/resources.hpp>
#include <NTTEngine/platforms/path.hpp>
#include "../ResourceTest.hpp"

using namespace ntt;

class ResourceManagerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        EventInit();
        ECSInit();
        ResourceInit(FALSE);

        ResourceTest::s_resources.clear();
    }

    void TearDown() override
    {
        ECSShutdown();
        EventShutdown();

        ResourceTest::s_resources.clear();
    }
};

TEST_F(ResourceManagerTest, TestResourceAutoUnload)
{
    ResourceLoad(
        {ResourceInfo{
            "test",
            ResourceType::TEST,
            JoinPath({GetFileFolder(__FILE__), "test.json"}, TRUE),
            JSON(R"(
                {}
            )")}});

    EXPECT_EQ(GET_LOAD_CALL("test"), 1);

    ResourceShutdown();
    ResourceTest::s_resources;
    EXPECT_EQ(GET_UNLOAD_CALL("test"), 1);
}

TEST_F(ResourceManagerTest, TestResourceUnloadManually)
{
    List<ResourceInfo> infos =
        {ResourceInfo{
            "test",
            ResourceType::TEST,
            JoinPath({GetFileFolder(__FILE__), "test.json"}, TRUE),
            JSON(R"(
                {}
            )")}};

    ResourceLoad(infos);

    ResourceTest::s_resources;
    EXPECT_EQ(GET_LOAD_CALL("test"), 1);

    ResourceUnload(infos);
    EXPECT_EQ(GET_UNLOAD_CALL("test"), 1);

    ResourceShutdown();
    EXPECT_EQ(GET_UNLOAD_CALL("test"), 1);
}