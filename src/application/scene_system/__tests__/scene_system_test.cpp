#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/scene_system/scene_system.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

using namespace ntt;
using namespace ecs;
using namespace event;

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
        SceneInit({
            {"default", DefaultSceneInit},
            {"next", NextSceneInit},
            {"large-scene-name", LargeSceneNameInit},
        });
    }

    void TearDown() override
    {
        SceneShutdown();
        ECSShutdown();
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
    EXPECT_EQ(entities, List<entity_id_t>({default_entity_1, default_entity_2}));

    auto event = RegisterEvent(
        NTT_SCENE_CHANGED,
        [](event_code_t code, void *sender, const EventContext &context)
        {
            auto scene_name = reinterpret_cast<const char *>(context.u8_data);
            EXPECT_STREQ(scene_name, "default");
        });

    SceneOpen("default");
    EXPECT_EQ(entities, List<entity_id_t>({default_entity_1, default_entity_2}));
    UnregisterEvent(event);

    event = RegisterEvent(
        NTT_SCENE_CHANGED,
        [](event_code_t code, void *sender, const EventContext &context)
        {
            auto scene_name = reinterpret_cast<const char *>(context.u8_data);
            EXPECT_STREQ(scene_name, "next");
        });
    SceneOpen("next");
    EXPECT_EQ(entities, List<entity_id_t>({next_scene_entity_1, next_scene_entity_2}));
    UnregisterEvent(event);

    b8 is_scene_opened = FALSE;

    event = RegisterEvent(
        NTT_SCENE_CHANGED,
        [&](event_code_t code, void *sender, const EventContext &context)
        {
            is_scene_opened = TRUE;
        });
    SceneOpen("non-exist");
    EXPECT_EQ(entities, List<entity_id_t>({next_scene_entity_1, next_scene_entity_2}));

    EXPECT_FALSE(is_scene_opened);
    UnregisterEvent(event);

    event = RegisterEvent(
        NTT_SCENE_CHANGED,
        [&](event_code_t code, void *sender, const EventContext &context)
        {
            auto scene_name = reinterpret_cast<const char *>(context.u8_data);
            const char *expected_scene_name = "large-scene-name";

            for (u32 i = 0; i < 16; i++)
            {
                if (i < 15)
                {
                    EXPECT_EQ(scene_name[i], expected_scene_name[i]);
                }
                else
                {
                    EXPECT_EQ(scene_name[i], '\0');
                }
            }
        });

    SceneOpen("large-scene-name");
    EXPECT_EQ(entities, List<entity_id_t>({}));
    UnregisterEvent(event);
}