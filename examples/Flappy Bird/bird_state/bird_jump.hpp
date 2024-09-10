#pragma once
#include <NTTEngine/NTTEngine.hpp>
#include "bird_state_defs.hpp"
#include "game_data.hpp"

class BirdJump : public State
{
protected:
    void OnEnterImpl() override
    {
        GetComponent<Geometry>()->rotatation = -35;
    }

    String OnNavigateImpl() override
    {
        if (GetComponent<Mass>()->velocity_y > 0)
        {
            return BIRD_FALL;
        }

        return KEEP_STATE;
    }
};

class BirdFall : public State
{
protected:
    void OnEnterImpl() override
    {
        GetComponent<Geometry>()->rotatation = 35;
    }

    String OnNavigateImpl() override
    {
        if (GetComponent<Mass>()->velocity_y < 0)
        {
            return BIRD_JUMP;
        }

        return KEEP_STATE;
    }
};