#pragma once
#include <NTTEngine/NTTEngine.hpp>

class SettingButtonController : public Script
{
protected:
    void OnEnterImpl() override
    {
        GetComponent<Hovering>()->callback =
            std::bind(&SettingButtonController::OnHover, this);
    }

    void OnHover()
    {
        if (CheckState(NTT_BUTTON_LEFT, NTT_PRESS))
        {
            LayerMakeVisible(LayerType::UI_LAYER);
        }
    }
};