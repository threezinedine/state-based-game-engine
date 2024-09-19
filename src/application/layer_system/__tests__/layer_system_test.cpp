#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/layer_system/layer_system.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

using namespace ntt;
using namespace ecs;
using namespace event;

class LayerSystemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        EventInit();
        ECSInit();
        LayerInit();
    }

    void TearDown() override
    {
        LayerShutdown();
        ECSShutdown();
        EventShutdown();
    }
};

TEST_F(LayerSystemTest, Needed_Entitites_Should_Be_Drawn)
{
    // At the beginning, the layer has default GAME_LAYER
    auto entity = ECSCreateEntity("test-entity", {});

    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity}));

    ECSDeleteEntity(entity);

    EXPECT_EQ(DrawnEntities(), List<entity_id_t>());
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>());

    entity = ECSCreateEntity("test-entity", {});

    BeginLayer(UI_LAYER_0);
    auto entity2 = ECSCreateEntity("test-entity-2", {});

    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity}));

    LayerMakeVisible(UI_LAYER_0);
    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity2}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity2}));

    BeginLayer(UI_LAYER_1);
    auto entity3 = ECSCreateEntity("test-entity-3", {});

    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity2}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity2}));

    LayerMakeVisible(UI_LAYER_1);
    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity3}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity3}));

    LayerMakeVisible(GAME_LAYER);
    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity}));

    BeginLayer(DEBUG_LAYER);
    auto entity4 = ECSCreateEntity("test-entity-4", {});
    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity}));

    LayerMakeVisible(DEBUG_LAYER);
    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity4}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity4}));

    LayerMakeVisible(GAME_LAYER);
    LayerMakeVisible(UI_LAYER_0);
    LayerMakeVisible(DEBUG_LAYER);
    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity2, entity4}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity4}));

    LayerMakeVisible(GAME_LAYER);

    // visible the layer which exceeds the MAX_LAYERS
    EXPECT_NO_THROW(LayerMakeVisible(DEBUG_LAYER + 1));
    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity}));

    // begin the layer which exceeds the MAX_LAYERS
    BeginLayer(GAME_LAYER);
    EXPECT_NO_THROW(BeginLayer(DEBUG_LAYER + 1));

    auto entity5 = ECSCreateEntity("test-entity-5", {});

    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity5}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity, entity5}));
}

TEST_F(LayerSystemTest, When_Change_The_Layer_Then_That_Layer_Trigger_Event)
{
    u8 layer = GAME_LAYER;

    auto callback = [&](const event_code_t code, void *sender, const EventContext &context)
    {
        layer = context.u8_data[0];
    };

    RegisterEvent(NTT_LAYER_CHANGED, callback);

    BeginLayer(UI_LAYER_0);

    ECSCreateEntity("test-entity", {});

    LayerMakeVisible(UI_LAYER_0);
    EXPECT_EQ(layer, UI_LAYER_0);

    LayerMakeVisible(UI_LAYER_1);
    EXPECT_EQ(layer, UI_LAYER_1);

    LayerMakeVisible(GAME_LAYER);
    EXPECT_EQ(layer, GAME_LAYER);

    LayerMakeVisible(DEBUG_LAYER);
    EXPECT_EQ(layer, DEBUG_LAYER);
}

TEST_F(LayerSystemTest, DebuggingBehaviorTest)
{
    BeginLayer(GAME_LAYER);
    auto entity = ECSCreateEntity("test-entity", {});
    auto entity2 = ECSCreateEntity("test-entity-2", {});

    BeginLayer(DEBUG_LAYER);
    auto entity3 = ECSCreateEntity("test-entity-3", {});
    auto entity4 = ECSCreateEntity("test-entity-4", {});

    EventContext context;

    LayerMakeVisible(GAME_LAYER);
    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity2}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity, entity2}));

    memset(context.u8_data, 0, sizeof(context.u8_data));
    TriggerEvent(NTT_DEBUG_BREAK, nullptr, context);

    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity2, entity3, entity4}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity3, entity4}));

    memset(context.u8_data, 0, sizeof(context.u8_data));
    context.b8_data[0] = TRUE;
    TriggerEvent(NTT_DEBUG_CONTINUE, nullptr, context);
    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity2, entity3, entity4}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity3, entity4}));

    memset(context.u8_data, 0, sizeof(context.u8_data));
    context.b8_data[0] = FALSE;
    TriggerEvent(NTT_DEBUG_CONTINUE, nullptr, context);
    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity2}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity, entity2}));

    // When the ui layer is visible, then the debug layer will return that layer when continue
    BeginLayer(UI_LAYER_0);
    auto entity5 = ECSCreateEntity("test-entity-5", {});

    LayerMakeVisible(UI_LAYER_0);
    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity2, entity5}));

    memset(context.u8_data, 0, sizeof(context.u8_data));
    TriggerEvent(NTT_DEBUG_BREAK, nullptr, context);

    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity2, entity5, entity3, entity4}));
    EXPECT_EQ(UpdatedEntities(), List<entity_id_t>({entity3, entity4}));

    memset(context.u8_data, 0, sizeof(context.u8_data));
    context.b8_data[0] = FALSE;
    TriggerEvent(NTT_DEBUG_CONTINUE, nullptr, context);

    EXPECT_EQ(DrawnEntities(), List<entity_id_t>({entity, entity2, entity5}));
}