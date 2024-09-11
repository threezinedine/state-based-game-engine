#include "game_playing.hpp"
#include "PipeController.hpp"
#include "defs.hpp"
#include "GameController.hpp"

#define CHANGE_PER (5)

String GamePlaying::OnNavigateImpl()
{
    if (GetComponent<GameData>()->isPlaying == FALSE)
    {
        return GAME_OVER_STATE;
    }

    return KEEP_STATE;
}

void GamePlaying::OnEnterImpl()
{
    auto windowSize = GetWindowSize();
    GetComponent<GameData>()->score = FALSE;

    TriggerEvent(PLAY_AGAIN_EVENT, nullptr, {0});
}