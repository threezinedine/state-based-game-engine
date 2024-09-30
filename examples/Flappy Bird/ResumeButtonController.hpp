#pragma once
#include <NTTEngine/NTTEngine.hpp>

class ResumeButtonController : public Script
{
protected:
    void OnEnterImpl() override
    {
        GetComponent<Hovering>()->callback =
            std::bind(&ResumeButtonController::OnHover, this);

        GetComponent<Hovering>()->onEnterCallback =
            std::bind(&ResumeButtonController::OnHoverEnter, this);

        GetComponent<Hovering>()->onExitCallback =
            std::bind(&ResumeButtonController::OnHoverExit, this);
    }

    void OnHoverEnter()
    {
        GetComponent<Texture>()->rowIndex = 5;
    }

    void OnHoverExit()
    {
        GetComponent<Texture>()->rowIndex = 1;
    }

    void OnHover()
    {
        if (CheckState(NTT_BUTTON_LEFT, NTT_PRESS))
        {
            OnClick();
        }
    }

    void OnClick()
    {
        ECSLayerMakeVisible(GAME_LAYER);
    }
};