#include "BirdController.hpp"

#define MOVE_UP_SPEED (0.3f)

void BirdController::OnCreate()
{
    CollisionRegister(
        GetEntity(),
        std::bind(&BirdController::OnCollide, this, std::placeholders::_1));
}

void BirdController::OnUpdate(f32 delta)
{
    if (CheckState(NTT_KEY_SPACE, NTT_PRESS))
    {
        GetComponent<Mass>()->velocity_y = -MOVE_UP_SPEED;
        PlayAudio(GetResourceID("wing"));
    }
}

void BirdController::OnCollide(List<entity_id_t> others)
{
    PlayAudio(GetResourceID("hit"));
    GetGameData()->state = GameState::GAME_OVER;
}