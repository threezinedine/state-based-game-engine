#pragma once
#include <NTTEngine/NTTEngine.hpp>

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::memory;

enum GameState
{
    START,
    PLAYING,
    GAME_OVER,
};

#define START_STATE "Start"
#define PLAYING_STATE "Playing"
#define GAME_OVER_STATE "GameOver"

struct GameData
{
    GameState state = START;
    u16 score = 0;
};

void InitGameData();
Ref<GameData> GetGameData();