#pragma once
#include <NTTEngine/NTTEngine.hpp>

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::memory;

enum GameState
{
    GAME_STATE_IDLE,
    GAME_STATE_PLAYING,
    GAME_STATE_OVER,
};

struct GameData : public ComponentBase
{
    GameState state;

    GameData()
        : state(GameState::GAME_STATE_IDLE) {}

    GameData(GameState state)
        : state(state) {}
};

void InitGameData();
Ref<GameData> GetGameData();