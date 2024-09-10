#pragma once
#include <NTTEngine/NTTEngine.hpp>
#include "game_data.hpp"
#include "bird_state_defs.hpp"

class BirdPlay : public State
{
public:
    void OnEnterImpl() override
    {
        GetComponent<Mass>()->velocity_y = -0.3f;
        auto geo = GetComponent<Geometry>();
        auto windowSize = GetWindowSize();

        geo->x = GetConfiguration().Get<f32>("bird_start_x", windowSize.width / 2);
        geo->y = GetWindowSize().height / 2;

        SetComponentActive<Geometry>(TRUE);
        SetComponentActive<Mass>(TRUE);
    }

    void OnUpdateImpl(f32 delta) override
    {
        if (CheckState(Key::NTT_KEY_SPACE, InputState::NTT_PRESS))
        {
            GetComponent<Mass>()->velocity_y = -0.3f;
        }
    }

    String OnNavigateImpl() override
    {
        if (GetGameData()->state == GameState::GAME_OVER)
        {
            return BIRD_DEAD;
        }

        return KEEP_STATE;
    }
};