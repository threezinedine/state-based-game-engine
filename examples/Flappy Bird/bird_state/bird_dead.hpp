#pragma once

#include <NTTEngine/NTTEngine.hpp>
#include "bird_state_defs.hpp"
#include "game_data.hpp"

class BirdDead : public State
{
protected:
    void OnEnterImpl() override
    {
        SetComponentActive<Mass>(FALSE);
    }

    String OnNavigateImpl() override
    {
        auto gameData = GetGameData();

        if (gameData->state == GameState::PLAYING)
        {
            return BIRD_PLAY;
        }

        return KEEP_STATE;
    }
};