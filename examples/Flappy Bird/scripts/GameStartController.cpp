#include "defs.hpp"
#include <NTTEngine/NTTEngine.hpp>

class GameStart : public Script
{
public:
    GameStart(void *data)
    {
    }

    ~GameStart()
    {
    }

protected:
    void OnEnterImpl() override
    {
        PlayAudio(GetResourceID("background-audio"), {0});
    }

    void OnUpdateImpl(f32 delta) override
    {
        if (CHECK_PRESS(NTT_KEY_ENTER))
        {
            SceneOpen("game");
        }
    }

    void OnExitImpl() override
    {
    }

private:
};

SCRIPT_DEFINE(GameStart, State)