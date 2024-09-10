#include "game_data.hpp"

namespace
{
    Ref<GameData> gameData;
}

void InitGameData()
{
    gameData = CreateRef<GameData>();
    gameData->state = GameState::START;
    gameData->score = 0;
}

Ref<GameData> GetGameData()
{
    return gameData;
}