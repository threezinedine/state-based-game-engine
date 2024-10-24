#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/physics/collision.hpp>
#include <NTTEngine/physics/collision_system.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/application/scene_system/scene_system.hpp>
#include <NTTEngine/renderer/renderer.hpp>
#include <NTTEngine/ecs/ecs.hpp>

using namespace ntt;

class CollisionTest : public testing::Test
{
protected:
    void SetUp() override
    {
        EventInit();
        ECSInit();

        ECSRegister(
            COLLISION_NAME,
            std::make_shared<CollisionSystem>(),
            {typeid(Collision), typeid(Geometry)});

        ECSBeginLayer(GAME_LAYER);
        ECSLayerMakeVisible(GAME_LAYER);

        SceneInit({
            {"default", {nullptr, nullptr}},
        });

        entity = ECSCreateEntity(
            "First",
            {
                ECS_CREATE_COMPONENT(Collision),
                ECS_CREATE_COMPONENT(Geometry, 0.0f, 0.0f, 1.0f, 1.0f),
            });

        entity2 = ECSCreateEntity(
            "Second",
            {
                ECS_CREATE_COMPONENT(Collision),
                ECS_CREATE_COMPONENT(Geometry, 1.0f, 1.0f, 1.0f, 1.0f),
            });

        entity3 = ECSCreateEntity(
            "Third",
            {
                ECS_CREATE_COMPONENT(Collision),
                ECS_CREATE_COMPONENT(Geometry, 2.0f, 2.0f, 1.0f, 1.0f),
            });

        collision = ECS_GET_COMPONENT(entity, Collision);
        collision->callback = std::bind(
            &CollisionTest::EntityCollide, this, std::placeholders::_1);

        collision2 = ECS_GET_COMPONENT(entity2, Collision);
        collision2->callback = std::bind(
            &CollisionTest::Entity2Collide, this, std::placeholders::_1);

        collision3 = ECS_GET_COMPONENT(entity3, Collision);
        collision3->callback = std::bind(
            &CollisionTest::Entity3Collide, this, std::placeholders::_1);

        geometry = ECS_GET_COMPONENT(entity, Geometry);
        geometry2 = ECS_GET_COMPONENT(entity2, Geometry);
        geometry3 = ECS_GET_COMPONENT(entity3, Geometry);
    }

    void TearDown() override
    {
        SceneShutdown();
        ECSShutdown();
        EventShutdown();
    }

    void EntityCollide(List<entity_id_t> entities)
    {
        entity_collision += entities.size();
    }

    void Entity2Collide(List<entity_id_t> entities)
    {
        entity2_collision += entities.size();
    }

    void Entity3Collide(List<entity_id_t> entities)
    {
        entity3_collision += entities.size();
    }

    entity_id_t entity;
    entity_id_t entity2;
    entity_id_t entity3;

    Ref<Collision> collision;
    Ref<Collision> collision2;
    Ref<Collision> collision3;

    Ref<Geometry> geometry;
    Ref<Geometry> geometry2;
    Ref<Geometry> geometry3;

    u8 entity_collision = 0;
    u8 entity2_collision = 0;
    u8 entity3_collision = 0;
};

TEST_F(CollisionTest, TestNormalPath)
{
    ECSUpdate(0.0f);

    EXPECT_EQ(entity_collision, 0);
    EXPECT_EQ(entity2_collision, 0);
    EXPECT_EQ(entity3_collision, 0);

    ECSUpdate(0.0f);

    EXPECT_EQ(entity_collision, 0);
    EXPECT_EQ(entity2_collision, 0);
    EXPECT_EQ(entity3_collision, 0);

    geometry->pos.x = 0.5f;
    geometry->pos.y = 0.5f;

    ECSUpdate(0.0f);

    EXPECT_EQ(entity_collision, 1);
    EXPECT_EQ(entity2_collision, 1);
    EXPECT_EQ(entity3_collision, 0);

    geometry2->pos.x = 0.5f;
    geometry2->pos.y = 0.5f;

    ECSUpdate(0.0f);

    EXPECT_EQ(entity_collision, 2);
    EXPECT_EQ(entity2_collision, 2);
    EXPECT_EQ(entity3_collision, 0);

    geometry3->pos.x = 0.5f;
    geometry3->pos.y = 0.5f;

    ECSUpdate(0.0f);

    EXPECT_EQ(entity_collision, 4);
    EXPECT_EQ(entity2_collision, 4);
    EXPECT_EQ(entity3_collision, 2);

    geometry->pos.x = -0.5f;
    geometry->pos.y = -0.5f;
    ECSUpdate(0.0f);

    EXPECT_EQ(entity_collision, 4);
    EXPECT_EQ(entity2_collision, 5);
    EXPECT_EQ(entity3_collision, 3);

    geometry2->pos.x = 2.0f;
    geometry2->pos.y = 2.0f;

    ECSUpdate(0.0f);

    EXPECT_EQ(entity_collision, 4);
    EXPECT_EQ(entity2_collision, 5);
    EXPECT_EQ(entity3_collision, 3);
}