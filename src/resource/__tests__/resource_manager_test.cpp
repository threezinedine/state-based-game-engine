#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/resources/resources.hpp>
#include <NTTEngine/platforms/path.hpp>
#include "../ResourceTest.hpp"

using namespace ntt;
using namespace ntt::event;
using namespace ntt::ecs;
using namespace ntt::resource;

class ResourceManagerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        EventInit();
        ECSInit();
        ResourceInit();

        ResourceTest::s_resources.clear();
    }

    void TearDown() override
    {
        ECSShutdown();
        EventShutdown();

        ResourceTest::s_resources.clear();
    }
};

TEST_F(ResourceManagerTest, TestNotChangeScene)
{
    RegisterResource(
        "default",
        ResourceInfo{
            "test",
            ResourceType::TEST,
            JoinPath({GetFileFolder(__FILE__), "test.json"}),
            JSON(R"(
                {}
            )")});

    ResourceStart();

    EXPECT_EQ(GET_LOAD_CALL("test"), 1);

    ResourceShutdown();
    ResourceTest::s_resources;
    EXPECT_EQ(GET_UNLOAD_CALL("test"), 1);
}

TEST_F(ResourceManagerTest, TestRegisterResource)
{
    RegisterResource(
        "scene1",
        ResourceInfo{
            "test",
            ResourceType::TEST,
            JoinPath({GetFileFolder(__FILE__), "test.json"}),
            JSON(R"(
                {}
            )")});

    ResourceStart();

    EXPECT_EQ(GET_LOAD_CALL("test"), 0);

    ChangeScene("scene1");
    EXPECT_EQ(GET_LOAD_CALL("test"), 1);
    EXPECT_EQ(GET_UNLOAD_CALL("test"), 0);

    ResourceShutdown();
    EXPECT_EQ(GET_UNLOAD_CALL("test"), 1);
}

TEST_F(ResourceManagerTest, RegisterAtDefaultScene)
{
    RegisterResource(
        "default",
        ResourceInfo{
            "test",
            ResourceType::TEST,
            JoinPath({GetFileFolder(__FILE__), "test.json"}),
            JSON(R"(
                {}
            )")});

    ResourceStart();

    EXPECT_EQ(GET_LOAD_CALL("test"), 1);

    ChangeScene("scene1");
    EXPECT_EQ(GET_LOAD_CALL("test"), 1);

    ResourceShutdown();

    EXPECT_EQ(GET_UNLOAD_CALL("test"), 1);
}

TEST_F(ResourceManagerTest, SceneChangingTest)
{
    ResourceLoadConfig(
        JSON(R"(
            {
                "default": [
                    {
                        "name": "test-default",
                        "type": 255,
                        "path": "test.json",
                        "data": {}
                    }
                ],
                "scene1": [
                    {
                        "name": "test-scene1",
                        "type": 255, 
                        "path": "test.json",
                        "data": {}
                    }
                ],
                "scene2": [
                    {
                        "name": "test-scene2",
                        "type": 255,
                        "path": "test.json",
                        "data": {}
                    }
                ]
            }
        )"));

    ResourceStart();

    EXPECT_EQ(GET_LOAD_CALL("test-default"), 1);
    EXPECT_EQ(GET_LOAD_CALL("test-scene1"), 0);
    EXPECT_EQ(GET_LOAD_CALL("test-scene2"), 0);

    EXPECT_EQ(GET_UNLOAD_CALL("test-default"), 0);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene1"), 0);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene2"), 0);

    ChangeScene("scene1");
    EXPECT_EQ(GET_LOAD_CALL("test-default"), 1);
    EXPECT_EQ(GET_LOAD_CALL("test-scene1"), 1);
    EXPECT_EQ(GET_LOAD_CALL("test-scene2"), 0);

    EXPECT_EQ(GET_UNLOAD_CALL("test-default"), 0);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene1"), 0);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene2"), 0);

    ChangeScene("scene2");
    EXPECT_EQ(GET_LOAD_CALL("test-default"), 1);
    EXPECT_EQ(GET_LOAD_CALL("test-scene1"), 1);
    EXPECT_EQ(GET_LOAD_CALL("test-scene2"), 1);

    EXPECT_EQ(GET_UNLOAD_CALL("test-default"), 0);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene1"), 1);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene2"), 0);

    ChangeScene("scene2");
    EXPECT_EQ(GET_LOAD_CALL("test-default"), 1);
    EXPECT_EQ(GET_LOAD_CALL("test-scene1"), 1);
    EXPECT_EQ(GET_LOAD_CALL("test-scene2"), 1);

    EXPECT_EQ(GET_UNLOAD_CALL("test-default"), 0);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene1"), 1);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene2"), 0);

    ChangeScene("scene1");
    EXPECT_EQ(GET_LOAD_CALL("test-default"), 1);
    EXPECT_EQ(GET_LOAD_CALL("test-scene1"), 2);
    EXPECT_EQ(GET_LOAD_CALL("test-scene2"), 1);

    EXPECT_EQ(GET_UNLOAD_CALL("test-default"), 0);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene1"), 1);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene2"), 1);

    ChangeScene("unknown");
    EXPECT_EQ(GET_LOAD_CALL("test-default"), 1);
    EXPECT_EQ(GET_LOAD_CALL("test-scene1"), 2);
    EXPECT_EQ(GET_LOAD_CALL("test-scene2"), 1);

    EXPECT_EQ(GET_UNLOAD_CALL("test-default"), 0);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene1"), 1);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene2"), 1);

    ChangeScene("");
    EXPECT_EQ(GET_LOAD_CALL("test-default"), 1);
    EXPECT_EQ(GET_LOAD_CALL("test-scene1"), 2);
    EXPECT_EQ(GET_LOAD_CALL("test-scene2"), 1);

    EXPECT_EQ(GET_UNLOAD_CALL("test-default"), 0);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene1"), 1);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene2"), 1);

    ResourceShutdown();

    EXPECT_EQ(GET_UNLOAD_CALL("test-default"), 1);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene1"), 2);
    EXPECT_EQ(GET_UNLOAD_CALL("test-scene2"), 1);
}