#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/scene_system/scene_system.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

using namespace ntt;

namespace
{
    entity_id_t default_entity_1;
    entity_id_t default_entity_2;

    entity_id_t next_scene_entity_1;
    entity_id_t next_scene_entity_2;

    List<entity_id_t> entities = {};
}

void DefaultSceneInit()
{
    default_entity_1 = ECSCreateEntity("default-entity", {});
    default_entity_2 = ECSCreateEntity("default-entity-2", {});
}

void NextSceneInit()
{
    next_scene_entity_1 = ECSCreateEntity("next-scene-entity", {});
    next_scene_entity_2 = ECSCreateEntity("next-scene-entity-2", {});
}

void LargeSceneNameInit()
{
}

class SceneSystemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        EventInit();
        RegisterEvent(
            NTT_ENTITY_CREATED,
            [&](const event_code_t code, void *sender, const EventContext &context)
            {
                auto entity_id = context.u32_data[0];
                entities.push_back(entity_id);
            });

        RegisterEvent(
            NTT_ENTITY_DESTROYED,
            [&](const event_code_t code, void *sender, const EventContext &context)
            {
                auto entity_id = context.u32_data[0];
                entities.RemoveItem(entity_id);
            });

        ECSInit();
    }

    void TearDown() override
    {
        SceneShutdown();
        ECSShutdown();
        EventShutdown();
    }
};

TEST(SceneSystemTest_Spec, CreateSceneWithEmptyList)
{
    ECSInit();
    SceneInit({});

    EXPECT_EQ(entities, List<entity_id_t>());

    SceneOpen("default");
    EXPECT_EQ(entities, List<entity_id_t>());

    SceneOpen("next");
    EXPECT_EQ(entities, List<entity_id_t>());

    SceneShutdown();
    ECSShutdown();
}

TEST_F(SceneSystemTest, AtTheBegging_TheSceneShouldInitTheDefaultScene)
{
    SceneInit({
        {"default", {DefaultSceneInit, nullptr}},
        {"next", {NextSceneInit, nullptr}},
        {"large-scene-name", {LargeSceneNameInit, nullptr}},
    });
    EXPECT_EQ(entities, List<entity_id_t>({default_entity_1, default_entity_2}));

    b8 hasCalled = FALSE;

    SceneOpen("default");
    ECSUpdate(0.0f);
    EXPECT_EQ(entities, List<entity_id_t>({default_entity_1, default_entity_2}));

    SceneOpen("next");
    ECSUpdate(0.0f);
    EXPECT_EQ(entities, List<entity_id_t>({next_scene_entity_1, next_scene_entity_2}));

    b8 is_scene_opened = FALSE;

    SceneOpen("non-exist");
    ECSUpdate(0.0f);
    EXPECT_EQ(entities, List<entity_id_t>({next_scene_entity_1, next_scene_entity_2}));

    EXPECT_FALSE(is_scene_opened);

    SceneOpen("large-scene-name");
    ECSUpdate(0.0f);
    EXPECT_EQ(entities, List<entity_id_t>({}));
}