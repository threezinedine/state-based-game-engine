#include "game_playing.hpp"
#include "PipeController.hpp"
#include "defs.hpp"
#include "GameController.hpp"

#define CHANGE_PER (5)

String GamePlaying::OnNavigateImpl()
{
    if (GetData().Get<b8>("isPlaying") == FALSE)
    {
        return GAME_OVER_STATE;
    }

    return KEEP_STATE;
}

void GamePlaying::OnEnterImpl()
{
    auto windowSize = GetWindowSize();
    GetData().Set<u32>("score", 0);

    TriggerEvent(PLAY_AGAIN_EVENT, nullptr, {0});
}