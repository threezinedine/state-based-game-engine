#include <NTTEngine/NTTEngine.hpp>

class ResumeButtonController : public Script
{
public:
    ResumeButtonController(void *data)
    {
    }
    ~ResumeButtonController()
    {
    }

protected:
    void OnEnterImpl() override
    {
    }

    void OnExitImpl() override
    {
    }

    void OnHoverEnter() override
    {
        GetComponent<TextureComponent>()->currentCell.row = 5;
    }

    void OnHoverExit() override
    {
        GetComponent<TextureComponent>()->currentCell.row = 1;
    }

    void OnHover(HoveringContext &context) override
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

SCRIPT_DEFINE(ResumeButtonController, Script)