#pragma once
#include <NTTEngine/NTTEngine.hpp>

class SettingButtonController : public Script
{
protected:
    void OnEnterImpl() override
    {
        GetComponent<Hovering>()->callback =
            std::bind(&SettingButtonController::OnHover, this);

        GetComponent<Hovering>()->onEnterCallback =
            std::bind(&SettingButtonController::OnHoverEnter, this);

        GetComponent<Hovering>()->onExitCallback =
            std::bind(&SettingButtonController::OnHoverExit, this);
    }

    void OnHoverEnter()
    {
        GetComponent<Texture>()->colIndex = 5;
    }

    void OnHoverExit()
    {
        GetComponent<Texture>()->colIndex = 11;
    }

    void OnHover()
    {
        if (CheckState(NTT_BUTTON_LEFT, NTT_PRESS))
        {
            LayerMakeVisible(UI_LAYER);
        }
    }
};