#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/renderer/Text.hpp>
#include <NTTEngine/renderer/Texture.hpp>

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::event;
using namespace ntt::renderer;

#define TEST_SYSTEM_NAME "TestSystem"

namespace
{
    u8 s_systemInitCalled = 0;
    u8 s_systemShutdownCalled = 0;
    List<entity_id_t> s_UpdatedEntities = {};
    List<entity_id_t> s_DrawnedEntites = {};

    void ResetEntityStates()
    {
        s_UpdatedEntities = {};
        s_DrawnedEntites = {};
    }
}

struct TestData : public ComponentBase
{
public:
    u8 initCalled = 0;
    u8 updateCalled = 0;
    u8 shutdownCalled = 0;
};

struct NonTestData : public ComponentBase
{
    u8 initCalled = 0;
    u8 updateCalled = 0;
    u8 shutdownCalled = 0;
};

struct TestLayerData : public ComponentBase
{
};

class TestUpdatedSystem : public System
{
public:
    void InitSystem() override
    {
        s_UpdatedEntities = {};
    }

    void Update(f32 delta, entity_id_t id) override
    {
        s_UpdatedEntities.push_back(id);
    }

    void ShutdownSystem() override
    {
        s_UpdatedEntities = {};
    }

    void InitEntity(entity_id_t id) override
    {
    }

    void ShutdownEntity(entity_id_t id) override
    {
    }
};

class TestDrawnSystem : public System
{
public:
    void InitSystem() override
    {
        s_DrawnedEntites = {};
    }

    void Update(f32 delta, entity_id_t id) override
    {
        s_DrawnedEntites.push_back(id);
    }

    void ShutdownSystem() override
    {
        s_DrawnedEntites = {};
    }

    void InitEntity(entity_id_t id) override
    {
    }

    void ShutdownEntity(entity_id_t id) override
    {
    }
};

class TestSystem : public System
{
public:
    void InitSystem() override { s_systemInitCalled++; }
    void InitEntity(entity_id_t id) override
    {
        auto data = ECS_GET_COMPONENT(id, TestData);
        data->initCalled++;
    }
    void Update(f32 delta, entity_id_t id) override
    {
        auto data = ECS_GET_COMPONENT(id, TestData);
        data->updateCalled++;
    }
    void ShutdownEntity(entity_id_t id) override
    {
        auto data = ECS_GET_COMPONENT(id, TestData);
        data->shutdownCalled++;
    }
    void ShutdownSystem() override { s_systemShutdownCalled++; }
};

class ECSTest : public testing::Test
{
protected:
    void SetUp() override
    {
        s_systemInitCalled = 0;
        s_systemShutdownCalled = 0;
        ResetEntityStates();

        EventInit();
        ECSInit();

        ECSRegister(TEST_SYSTEM_NAME, CreateRef<TestSystem>(), {typeid(TestData)});
        ECSRegister(
            "TestUpdatedSystem",
            CreateRef<TestUpdatedSystem>(),
            {typeid(TestLayerData)});
        ECSRegister(
            "TestDrawnSystem",
            CreateRef<TestDrawnSystem>(),
            {typeid(TestLayerData)},
            TRUE);
        EXPECT_EQ(s_systemInitCalled, 1);
        EXPECT_EQ(s_systemShutdownCalled, 0);

        ECSBeginLayer(GAME_LAYER);
        ECSLayerMakeVisible(GAME_LAYER);

        entity = ECSCreateEntity(
            "TestEntity",
            {ECS_CREATE_COMPONENT(TestData)});

        data = ECS_GET_COMPONENT(entity, TestData);

        entity2 = ECSCreateEntity(
            "TestEntity2",
            {ECS_CREATE_COMPONENT(TestData)});

        data2 = ECS_GET_COMPONENT(entity2, TestData);

        entity4 = ECSCreateEntity(
            "TestEntity4",
            {ECS_CREATE_COMPONENT(NonTestData)});

        data4 = ECS_GET_COMPONENT(entity4, NonTestData);
    }

    void TearDown() override
    {
        ECSShutdown();
        EventShutdown();

        EXPECT_EQ(data2->shutdownCalled, 1);
        EXPECT_EQ(s_systemInitCalled, 1);
        EXPECT_EQ(s_systemShutdownCalled, 1);
    }

    template <typename T>
    void Check2ListEquivalent(List<T> lst1, List<T> lst2)
    {
        EXPECT_EQ(lst1.size(), lst2.size());
        for (auto i = 0; i < lst1.size(); i++)
        {
            EXPECT_TRUE(lst1.Contains(lst2[i]));
        }
    }

    entity_id_t entity;
    entity_id_t entity2;
    entity_id_t entity4;

    Ref<TestData> data;
    Ref<TestData> data2;
    Ref<NonTestData> data4;
};

TEST_F(ECSTest, TestECSInit)
{
    EXPECT_EQ(data->initCalled, 1);
    EXPECT_EQ(data->updateCalled, 0);
    EXPECT_EQ(data->shutdownCalled, 0);

    ECSUpdate(0.0f);
    ECSUpdate(0.0f);

    EXPECT_EQ(data->initCalled, 1);
    EXPECT_EQ(data->updateCalled, 2);
    EXPECT_EQ(data->shutdownCalled, 0);

    EXPECT_EQ(ECSGetEntitiesWithSystem(TEST_SYSTEM_NAME),
              List<entity_id_t>({entity, entity2}));

    entity_id_t eventEntity = 0;

    auto event = RegisterEvent(
        NTT_ENTITY_CREATED,
        [&eventEntity](auto code, auto sender, const EventContext &context)
        { eventEntity = context.u32_data[0]; });

    auto entity3 = ECSCreateEntity(
        "TestEntity3",
        {ECS_CREATE_COMPONENT(TestData)});

    EXPECT_EQ(eventEntity, entity3);

    UnregisterEvent(event);

    auto data3 = ECS_GET_COMPONENT(entity3, TestData);

    EXPECT_EQ(data3->initCalled, 1);
    EXPECT_EQ(data3->updateCalled, 0);
    EXPECT_EQ(data3->shutdownCalled, 0);

    event = RegisterEvent(
        NTT_ENTITY_DESTROYED,
        [&eventEntity](auto code, auto sender, const EventContext &context)
        { eventEntity = context.u32_data[0]; });

    ECSDeleteEntity(entity);
    ECSUpdate(0.0f);
    EXPECT_EQ(eventEntity, entity);
    UnregisterEvent(event);

    EXPECT_EQ(ECSGetEntitiesWithSystem(TEST_SYSTEM_NAME),
              List<entity_id_t>({entity2, entity3}));

    EXPECT_EQ(data->initCalled, 1);
    EXPECT_EQ(data->updateCalled, 3);
    EXPECT_EQ(data->shutdownCalled, 1);

    EXPECT_EQ(data3->initCalled, 1);
    EXPECT_EQ(data3->updateCalled, 1);
    EXPECT_EQ(data3->shutdownCalled, 0);

    ECSUpdate(0.0f);
    EXPECT_EQ(data3->initCalled, 1);
    EXPECT_EQ(data3->updateCalled, 2);
    EXPECT_EQ(data3->shutdownCalled, 0);

    EXPECT_EQ(data4->initCalled, 0);
    EXPECT_EQ(data4->updateCalled, 0);
    EXPECT_EQ(data4->shutdownCalled, 0);

    ECSDeleteEntity(entity4);
    EXPECT_EQ(data4->initCalled, 0);
    EXPECT_EQ(data4->updateCalled, 0);
    EXPECT_EQ(data4->shutdownCalled, 0);

    EXPECT_EQ(ECSGetEntitiesWithSystem(TEST_SYSTEM_NAME),
              List<entity_id_t>({entity2, entity3}));

    EXPECT_NO_THROW(ECSDeleteEntity(0x34));
}

TEST_F(ECSTest, TestComponentActive)
{
    ECSUpdate(0.0f);
    ECSUpdate(0.0f);

    EXPECT_EQ(data->initCalled, 1);
    EXPECT_EQ(data->updateCalled, 2);
    EXPECT_EQ(data->shutdownCalled, 0);

    EXPECT_EQ(data2->initCalled, 1);
    EXPECT_EQ(data2->updateCalled, 2);
    EXPECT_EQ(data2->shutdownCalled, 0);

    EXPECT_NO_THROW(ECSSetComponentActive(0x45, typeid(TestData), FALSE));
    ECSSetComponentActive(entity, typeid(TestData), FALSE);

    Check2ListEquivalent(ECSGetEntitiesWithSystem(TEST_SYSTEM_NAME), {entity2});

    ECSUpdate(0.0f);
    ECSUpdate(0.0f);

    EXPECT_EQ(data->initCalled, 1);
    EXPECT_EQ(data->updateCalled, 2);
    EXPECT_EQ(data->shutdownCalled, 0);

    EXPECT_EQ(data2->initCalled, 1);
    EXPECT_EQ(data2->updateCalled, 4);
    EXPECT_EQ(data2->shutdownCalled, 0);

    ECSSetComponentActive(entity, typeid(TestData), TRUE);

    ECSUpdate(0.0f);
    ECSUpdate(0.0f);

    EXPECT_EQ(data->initCalled, 1);
    EXPECT_EQ(data->updateCalled, 4);
    EXPECT_EQ(data->shutdownCalled, 0);

    EXPECT_EQ(data2->initCalled, 1);
    EXPECT_EQ(data2->updateCalled, 6);
    EXPECT_EQ(data2->shutdownCalled, 0);

    EXPECT_NO_THROW(ECSSetComponentActive(entity4, typeid(TestData), TRUE));

    ECSSetComponentActive(entity4, typeid(NonTestData), TRUE);
    Check2ListEquivalent(ECSGetEntitiesWithSystem(TEST_SYSTEM_NAME), {entity, entity2});
}

TEST_F(ECSTest, Needed_Entitites_Should_Be_Drawn)
{
    // At the beginning, the layer has default GAME_LAYER
    auto entity = ECSCreateEntity(
        "test-entity",
        {
            ECS_CREATE_COMPONENT(TestLayerData),
        });

    ECSUpdate(0.0f);

    EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity}));
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity}));
    ResetEntityStates();

    ECSDeleteEntity(entity);

    ECSUpdate(0.0f);
    ResetEntityStates();
    ECSUpdate(0.0f);

    EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>());
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>());
    ResetEntityStates();

    entity = ECSCreateEntity(
        "test-entity",
        {
            ECS_CREATE_COMPONENT(TestLayerData),
        });

    ECSBeginLayer(UI_LAYER_0);
    auto entity2 = ECSCreateEntity(
        "test-entity-2",
        {
            ECS_CREATE_COMPONENT(TestLayerData),
        });

    ECSUpdate(0.0f);

    EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity}));
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity}));
    ResetEntityStates();

    ECSLayerMakeVisible(UI_LAYER_0);
    ECSUpdate(0.0f);
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity, entity2}));
    EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity2}));
    ResetEntityStates();

    ECSBeginLayer(UI_LAYER_1);
    auto entity3 = ECSCreateEntity(
        "test-entity-3",
        {
            ECS_CREATE_COMPONENT(TestLayerData),
        });

    ECSUpdate(0.0f);
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity, entity2}));
    EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity2}));
    ResetEntityStates();

    ECSLayerMakeVisible(UI_LAYER_1);
    ECSUpdate(0.0f);
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity, entity3}));
    EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity3}));
    ResetEntityStates();

    ECSLayerMakeVisible(GAME_LAYER);
    ECSUpdate(0.0f);
    EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity}));
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity}));
    ResetEntityStates();

    ECSBeginLayer(EDITOR_LAYER);
    auto entity4 = ECSCreateEntity(
        "test-entity-4",
        {
            ECS_CREATE_COMPONENT(TestLayerData),
        });
    ECSUpdate(0.0f);
    EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity}));
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity}));
    ResetEntityStates();

    ECSLayerMakeVisible(EDITOR_LAYER);
    ECSUpdate(0.0f);
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity, entity4}));
    EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity4}));
    ResetEntityStates();

    ECSLayerMakeVisible(GAME_LAYER);
    ECSLayerMakeVisible(UI_LAYER_0);
    ECSLayerMakeVisible(EDITOR_LAYER);
    ECSUpdate(0.0f);
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity, entity2, entity4}));
    EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity4}));
    ResetEntityStates();

    ECSLayerMakeVisible(GAME_LAYER);
    ECSUpdate(0.0f);
    ResetEntityStates();

    // visible the layer which exceeds the MAX_LAYERS
    EXPECT_NO_THROW(ECSLayerMakeVisible(EDITOR_LAYER + 1));
    ECSUpdate(0.0f);
    EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity}));
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity}));
    ResetEntityStates();

    // begin the layer which exceeds the MAX_LAYERS
    ECSBeginLayer(GAME_LAYER);
    ECSUpdate(0.0f);
    ResetEntityStates();
    EXPECT_NO_THROW(ECSBeginLayer(EDITOR_LAYER + 1));
    ResetEntityStates();

    auto entity5 = ECSCreateEntity(
        "test-entity-5",
        {
            ECS_CREATE_COMPONENT(TestLayerData),
        });

    ECSUpdate(0.0f);
    EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity, entity5}));
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity, entity5}));
    ResetEntityStates();
}

TEST_F(ECSTest, When_Change_The_Layer_Then_That_Layer_Trigger_Event)
{
    u8 layer = GAME_LAYER;

    auto callback = [&](const event_code_t code, void *sender, const EventContext &context)
    {
        layer = context.u8_data[0];
    };

    RegisterEvent(NTT_LAYER_CHANGED, callback);

    ECSBeginLayer(UI_LAYER_0);

    ECSCreateEntity("test-entity", {});

    ECSLayerMakeVisible(UI_LAYER_0);
    EXPECT_EQ(layer, UI_LAYER_0);

    ECSLayerMakeVisible(UI_LAYER_1);
    EXPECT_EQ(layer, UI_LAYER_1);

    ECSLayerMakeVisible(GAME_LAYER);
    EXPECT_EQ(layer, GAME_LAYER);

    ECSLayerMakeVisible(EDITOR_LAYER);
    EXPECT_EQ(layer, EDITOR_LAYER);
}

TEST_F(ECSTest, DebuggingBehaviorTest)
{
    ECSBeginLayer(GAME_LAYER);
    auto entity = ECSCreateEntity(
        "test-entity",
        {
            ECS_CREATE_COMPONENT(TestLayerData),
        });
    auto entity2 = ECSCreateEntity(
        "test-entity-2",
        {
            ECS_CREATE_COMPONENT(TestLayerData),
        });

    ECSBeginLayer(EDITOR_LAYER);
    auto entity3 = ECSCreateEntity(
        "test-entity-3",
        {
            ECS_CREATE_COMPONENT(TestLayerData),
        });
    auto entity4 = ECSCreateEntity(
        "test-entity-4",
        {
            ECS_CREATE_COMPONENT(TestLayerData),
        });

    EventContext context;

    // ECSLayerMakeVisible(GAME_LAYER);
    // ECSUpdate(0.0f);
    // EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity, entity2}));
    // EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity, entity2}));
    // ResetEntityStates();

    // memset(context.u8_data, 0, sizeof(context.u8_data));
    // // TriggerEvent(NTT_DEBUG_BREAK, nullptr, context);

    // ECSUpdate(0.0f);
    // EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity, entity2, entity3, entity4}));
    // EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity3, entity4}));
    // ResetEntityStates();

    // memset(context.u8_data, 0, sizeof(context.u8_data));
    // context.b8_data[0] = TRUE;
    // // TriggerEvent(NTT_DEBUG_CONTINUE, nullptr, context);
    // ECSUpdate(0.0f);
    // EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity, entity2, entity3, entity4}));
    // EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity3, entity4}));
    // ResetEntityStates();

    // memset(context.u8_data, 0, sizeof(context.u8_data));
    // context.b8_data[0] = FALSE;
    // // TriggerEvent(NTT_DEBUG_CONTINUE, nullptr, context);
    // ECSUpdate(0.0f);
    // EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity, entity2}));
    // EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity, entity2}));
    // ResetEntityStates();

    // // When the ui layer is visible, then the debug layer will return that layer when continue
    // ECSBeginLayer(UI_LAYER_0);
    // ECSUpdate(0.0f);
    // ResetEntityStates();
    // auto entity5 = ECSCreateEntity(
    //     "test-entity-5",
    //     {
    //         ECS_CREATE_COMPONENT(TestLayerData),
    //     });

    // ECSLayerMakeVisible(UI_LAYER_0);
    // ECSUpdate(0.0f);
    // EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity, entity2, entity5}));
    // ResetEntityStates();

    // memset(context.u8_data, 0, sizeof(context.u8_data));
    // // TriggerEvent(NTT_DEBUG_BREAK, nullptr, context);
    // ECSUpdate(0.0f);
    // EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity,
    //                                                entity2,
    //                                                entity3,
    //                                                entity4,
    //                                                entity5}));
    // EXPECT_EQ(s_UpdatedEntities, List<entity_id_t>({entity3, entity4}));
    // ResetEntityStates();

    // memset(context.u8_data, 0, sizeof(context.u8_data));
    // context.b8_data[0] = FALSE;
    // // TriggerEvent(NTT_DEBUG_CONTINUE, nullptr, context);
    // ECSUpdate(0.0f);
    // EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({entity, entity2, entity5}));
    // ResetEntityStates();
}

TEST_F(ECSTest, CreateEntityWithPriority)
{
    ECSBeginLayer(GAME_LAYER);

    auto textureEnt = ECSCreateEntity(
        "test-entity",
        {
            ECS_CREATE_COMPONENT(Texture, 1, 0, 0, PRIORITY_0),
            ECS_CREATE_COMPONENT(TestLayerData),
        });

    auto textEnt = ECSCreateEntity(
        "test-entity-2",
        {
            ECS_CREATE_COMPONENT(Text, "test", 10, PRIORITY_1),
            ECS_CREATE_COMPONENT(TestLayerData),
        });

    auto textEntTexture = ECS_GET_COMPONENT(textureEnt, Texture);
    EXPECT_EQ(textEntTexture->priority, PRIORITY_0);

    auto textEntText = ECS_GET_COMPONENT(textEnt, Text);
    EXPECT_EQ(textEntText->priority, PRIORITY_1);

    TriggerEvent(NTT_ENTITY_CREATED, nullptr, {textEnt});
    ECSUpdate(0.0f);
    EXPECT_EQ(s_DrawnedEntites, List<entity_id_t>({textureEnt, textEnt}));
    ResetEntityStates();

    ECSBeginLayer(UI_LAYER_0);

    auto textureEnt2 = ECSCreateEntity(
        "test-entity-3",
        {
            ECS_CREATE_COMPONENT(Texture, 1, 0, 0, PRIORITY_0),
        });

    auto textEnt2 = ECSCreateEntity(
        "test-entity-4",
        {
            ECS_CREATE_COMPONENT(Text, "test", 10, PRIORITY_1),
        });

    auto textEntTexture2 = ECS_GET_COMPONENT(textureEnt2, Texture);
    EXPECT_EQ(textEntTexture2->priority, PRIORITY_0 + LAYER_PRIORITY_RANGE * UI_LAYER_0);

    auto textEntText2 = ECS_GET_COMPONENT(textEnt2, Text);
    EXPECT_EQ(textEntText2->priority, PRIORITY_1 + LAYER_PRIORITY_RANGE * UI_LAYER_0);
}