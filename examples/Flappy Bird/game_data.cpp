#include "game_data.hpp"

namespace
{
    Ref<GameData> gameData;
}

void InitGameData()
{
    gameData = CreateRef<GameData>();
}

Ref<GameData> GetGameData()
{
    return gameData;
}