#include <NTTEngine/NTTEngine.hpp>

class SettingButtonController : public Script
{
public:
    SettingButtonController(void *data)
    {
    }

    ~SettingButtonController()
    {
    }

protected:
    void OnEnterImpl() override
    {
    }

    void OnHoverEnter() override
    {
        GetComponent<TextureComponent>()->currentCell.col = 5;
    }

    void OnHoverExit() override
    {
        GetComponent<TextureComponent>()->currentCell.col = 11;
    }

    void OnHover(HoveringContext &context) override
    {
        if (CheckState(NTT_BUTTON_LEFT, NTT_PRESS))
        {
            ECSLayerMakeVisible(UI_LAYER_0);
        }
    }
};

SCRIPT_DEFINE(SettingButtonController, Script)