#include "game_playing.hpp"
#include "game_data.hpp"

void GamePlaying::OnUpdateImpl(f32 delta)
{
    // NTT_APP_INFO("In game playing state.");
}

String GamePlaying::OnNavigateImpl()
{
    if (GetGameData()->state == GameState::GAME_OVER)
    {
        return GAME_OVER_STATE;
    }

    return KEEP_STATE;
}