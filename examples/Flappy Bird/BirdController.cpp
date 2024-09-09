#include "BirdController.hpp"
#include "game_data.hpp"

using namespace ntt::physics;
using namespace ntt::ecs;
using namespace ntt::memory;
using namespace ntt::log;
using namespace ntt::renderer;
using namespace ntt::input;
using namespace ntt::audio;

void BirdController::OnCreate()
{
    SetComponentState<Mass>(FALSE);
    SetComponentState<Sprite>(FALSE);
    SetComponentState<Texture>(FALSE);

    GetComponent<Geometry>()->x = GetConfiguration().Get<position_t>("start-bird-x", 200);
}

void BirdController::OnUpdate(f32 deltaTime)
{
    auto game = GetGameData();

    auto geo = GetComponent<Geometry>();
    auto mass = GetComponent<Mass>();
    auto birdAngle = GetConfiguration().Get<f32>("bird-angle", 35);

    if (mass->velocity_y > 0)
    {
        geo->rotatation = birdAngle;
    }
    else if (mass->velocity_y == 0)
    {
        geo->rotatation = 0;
    }
    else
    {
        geo->rotatation = -birdAngle;
    }

    if (CheckState(Key::NTT_KEY_SPACE, InputState::NTT_PRESS))
    {
        mass->velocity_y = -0.3;
        PlayAudio(GetResourceID("wing"));
    }
}

void BirdController::OnDestroy()
{
}