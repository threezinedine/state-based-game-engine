#include "game_over.hpp"
#include "defs.hpp"

using namespace ntt::input;
using namespace ntt::log;

void GameOver::OnEnterImpl()
{
    NTT_APP_INFO("Game Over");
    auto windowSize = GetWindowSize();
    TriggerEvent(GAME_OVER_EVENT, nullptr, {0});

    if (m_gameOverMessage == INVALID_ENTITY_ID)
    {
        m_gameOverMessage = ECSCreateEntity(
            "game-over-message",
            {
                ECS_CREATE_COMPONENT(Geometry,
                                     windowSize.width / 2,
                                     windowSize.height / 2,
                                     300,
                                     100),
                ECS_CREATE_COMPONENT(Texture, GetResourceID("gameover"), 0, 0, PRIORITY_4),
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
    if (CheckState(NTT_KEY_ENTER, NTT_DOWN))
    {
        return PLAYING_STATE;
    }

    return KEEP_STATE;
}
