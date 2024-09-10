#include "game_over.hpp"
#include "game_data.hpp"

using namespace ntt::input;
using namespace ntt::log;

void GameOver::OnEnterImpl()
{
    auto windowSize = GetWindowSize();

    if (m_gameOverMessage == INVALID_ENTITY_ID)
    {
        m_gameOverMessage = ECSCreateEntity(
            "Game Over",
            {
                ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, windowSize.height / 2, 300, 100),
                ECS_CREATE_COMPONENT(Texture, GetResourceID("gameover")),
            });
    }

    ECSSetComponentActive(m_gameOverMessage, typeid(Geometry), TRUE);
}

void GameOver::OnUpdateImpl(f32 delta)
{
}

void GameOver::OnExitImpl()
{
    ECSSetComponentActive(m_gameOverMessage, typeid(Geometry), FALSE);
}

String GameOver::OnNavigateImpl()
{
    if (GetGameData()->state == GameState::PLAYING)
    {
        return PLAYING_STATE;
    }

    return KEEP_STATE;
}
