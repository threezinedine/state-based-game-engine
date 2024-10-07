#include "defs.hpp"
#include <NTTEngine/NTTEngine.hpp>
#define CHANGE_PER (5)

class GamePlaying : public State
{
public:
    GamePlaying(void *data)
    {
    }

    ~GamePlaying()
    {
    }

protected:
    void OnEnterImpl() override
    {
        NTT_APP_INFO("Playing game");
        auto windowSize = GetWindowSize();
        GetData().Set<u32>("score", 0);

        TriggerEvent(PLAY_AGAIN_EVENT, nullptr, {0});
    }

    String OnNavigateImpl() override
    {
        if (GetData().Get<b8>("isPlaying") == FALSE)
        {
            return GAME_OVER_STATE;
        }

        return KEEP_STATE;
    }
};

SCRIPT_DEFINE(GamePlaying, State)