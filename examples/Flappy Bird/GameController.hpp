#pragma once
#include <NTTEngine/NTTEngine.hpp>

#define START_STATE "Start"
#define PLAYING_STATE "Playing"
#define GAME_OVER_STATE "GameOver"

struct GameData : public ComponentBase
{
    b8 isPlaying = FALSE;
    u16 score = 0;

    GameData() = default;
};

class GameController : public Script
{
protected:
    void OnCreateImpl() override;

    void AddNewPipe(position_t posX);

private:
};