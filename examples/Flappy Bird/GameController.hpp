#pragma once
#include <NTTEngine/NTTEngine.hpp>

#define START_STATE "Start"
#define PLAYING_STATE "Playing"
#define GAME_OVER_STATE "GameOver"

class GameController : public Script
{
protected:
    void OnCreateImpl() override;

    void AddNewPipe(position_t posX);

private:
};