#pragma once
#include <NTTEngine/NTTEngine.hpp>
#include "game_data.hpp"
#include "bird_state_defs.hpp"

class BirdStart : public State
{
protected:
    void OnEnterImpl() override
    {
        NTT_APP_INFO("Current Id: {}", GetId());

        GetComponent<Mass>()->AddForceConst(
            0, GetConfiguration().Get<f32>("bird-gravity", 0.01f));

        SetComponentActive<Geometry>(FALSE);
        SetComponentActive<Mass>(FALSE);
        SetComponentActive<Sprite>(FALSE);
    }

    void OnExitImpl() override
    {
        SetComponentActive<Geometry>(TRUE);
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