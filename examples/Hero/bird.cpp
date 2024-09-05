#include "bird.hpp"

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::renderer;
using namespace ntt::physics;
using namespace ntt::input;

static texture_id_t m_birdTexture = 0;

static f32 force = 0.001;

enum BirdStateType
{
    BIRD_STATE_IDLE,
    BIRD_STATE_FLYING,
    BIRD_STATE_FALLING,
};

struct BirdState : public ComponentBase
{
    BirdStateType state;
    Timer timer;

    BirdState(BirdStateType state = BIRD_STATE_IDLE)
        : state(BIRD_STATE_IDLE)
    {
        timer.Reset();
    }
};

void BirdSystemFunc(f32 delta, entity_id_t id, List<entity_id_t> others);
void MarginFunc(f32 data, entity_id_t id, List<entity_id_t> others);

Bird::Bird()
{
    m_birdTexture = LoadTexture(RelativePath("images/game-objects/bird.png"), {3, 1});

    ECSRegister(
        "BirdSystem",
        BirdSystemFunc,
        {typeid(BirdState), typeid(Texture), typeid(Mass)});
    ECSRegister(
        "MarginSystem",
        MarginFunc,
        {typeid(Geometry)});
}

Bird::~Bird()
{
}

void Bird::CreateBird()
{
    auto mass = CreateRef<Mass>(1.0f);
    mass->AddForceConst(0, force);

    ECSCreateEntity(
        "Bird",
        {ECS_CREATE_COMPONENT(Geometry, 200, 200, 70),
         ECS_CREATE_COMPONENT(Texture, m_birdTexture),
         {typeid(Mass), mass},
         ECS_CREATE_COMPONENT(BirdState, BirdStateType::BIRD_STATE_IDLE)});
}

void BirdSystemFunc(f32 delta, entity_id_t id, List<entity_id_t> others)
{
    auto state = ECS_GET_COMPONENT(id, BirdState);
    auto texture = ECS_GET_COMPONENT(id, Texture);
    auto geo = ECS_GET_COMPONENT(id, Geometry);
    auto mass = ECS_GET_COMPONENT(id, Mass);

    NTT_APP_DEBUG("Running bird State");

    if (state->timer.GetMilliseconds() >= 200)
    {
        state->timer.Reset();

        switch (state->state)
        {
        case BirdStateType::BIRD_STATE_IDLE:
            state->state = BirdStateType::BIRD_STATE_FLYING;
            texture->rowIndex = 2;
            break;
        case BirdStateType::BIRD_STATE_FLYING:
            state->state = BirdStateType::BIRD_STATE_FALLING;
            texture->rowIndex = 0;
            break;
        default:
            state->state = BirdStateType::BIRD_STATE_IDLE;
            texture->rowIndex = 1;
        }
    }

    if (mass->velocity_y > 0)
    {
        geo->rotatation = 35;
    }

    if (mass->velocity_y == 0)
    {
        geo->rotatation = 0;
    }

    if (mass->velocity_y < 0)
    {
        geo->rotatation = -35;
    }

    if (CheckState(Key::NTT_KEY_SPACE, InputState::NTT_PRESS))
    {
        mass->velocity_y = -0.3;
    }
}

void MarginFunc(f32 data, entity_id_t id, List<entity_id_t> others)
{
    auto geo = ECS_GET_COMPONENT(id, Geometry);
    auto windowSize = GetWindowSize();

    NTT_APP_DEBUG("Current position: x: {}, y: {}", geo->x, geo->y);

    if (geo->y - geo->height / 2 < 0)
    {
        ECSSetComponentActive(id, typeid(Mass), FALSE);
    }
    else if (geo->y + geo->height / 2 > windowSize.height)
    {
        ECSSetComponentActive(id, typeid(Mass), FALSE);
    }
}