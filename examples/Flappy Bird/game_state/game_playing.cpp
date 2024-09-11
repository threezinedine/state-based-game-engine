#include "game_playing.hpp"
#include "game_data.hpp"
#include "PipeController.hpp"

String GamePlaying::OnNavigateImpl()
{
    if (GetGameData()->state == GameState::GAME_OVER)
    {
        return GAME_OVER_STATE;
    }

    return KEEP_STATE;
}

void GamePlaying::OnEnterImpl()
{
    ResetPipe();

    auto windowSize = GetWindowSize();
    GetGameData()->score = 0;
    CreatePipe(windowSize.width + 100);
}