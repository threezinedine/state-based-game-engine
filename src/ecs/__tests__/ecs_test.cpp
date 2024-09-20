#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/application/layer_system/layer_system.hpp>

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::event;

#define TEST_SYSTEM_NAME "TestSystem"

namespace
{
    u8 s_systemInitCalled = 0;
    u8 s_systemShutdownCalled = 0;
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

class TestSystem : public System
{
public:
    TestSystem() : System() {}
    virtual ~TestSystem() noexcept override {}

protected:
    void InitSystemImpl() override { s_systemInitCalled++; }
    void InitEntityImpl(entity_id_t id) override
    {
        auto data = ECS_GET_COMPONENT(id, TestData);
        data->initCalled++;
    }
    void UpdateImpl(f32 delta, entity_id_t id) override
    {
        auto data = ECS_GET_COMPONENT(id, TestData);
        data->updateCalled++;
    }
    void ShutdownEntityImpl(entity_id_t id) override
    {
        auto data = ECS_GET_COMPONENT(id, TestData);
        data->shutdownCalled++;
    }
    void ShutdownSystemImpl() override { s_systemShutdownCalled++; }
};

class ECSTest : public testing::Test
{
protected:
    void SetUp() override
    {
        s_systemInitCalled = 0;
        s_systemShutdownCalled = 0;

        EventInit();
        ECSInit();
        LayerInit();

        ECSRegister(TEST_SYSTEM_NAME, std::make_shared<TestSystem>(), {typeid(TestData)});
        EXPECT_EQ(s_systemInitCalled, 1);
        EXPECT_EQ(s_systemShutdownCalled, 0);

        BeginLayer(GAME_LAYER);
        LayerMakeVisible(GAME_LAYER);

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
        LayerShutdown();
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
    EXPECT_EQ(eventEntity, entity);
    UnregisterEvent(event);

    EXPECT_EQ(ECSGetEntitiesWithSystem(TEST_SYSTEM_NAME),
              List<entity_id_t>({entity2, entity3}));

    EXPECT_EQ(data->initCalled, 1);
    EXPECT_EQ(data->updateCalled, 2);
    EXPECT_EQ(data->shutdownCalled, 1);

    EXPECT_EQ(data3->initCalled, 1);
    EXPECT_EQ(data3->updateCalled, 0);
    EXPECT_EQ(data3->shutdownCalled, 0);

    ECSUpdate(0.0f);
    EXPECT_EQ(data3->initCalled, 1);
    EXPECT_EQ(data3->updateCalled, 1);
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