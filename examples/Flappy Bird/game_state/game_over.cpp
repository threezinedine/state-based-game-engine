#include "game_over.hpp"
#include "PipeController.hpp"
#include "defs.hpp"
#include "GameController.hpp"

using namespace ntt::input;
using namespace ntt::log;

void GameOver::OnEnterImpl()
{
    auto windowSize = GetWindowSize();
    TriggerEvent(GAME_OVER_EVENT, nullptr, {0});

    if (m_gameOverMessage == INVALID_ENTITY_ID)
    {
        m_gameOverMessage = ECSCreateEntity(
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
    if (CheckState(NTT_KEY_ENTER, NTT_DOWN))
    {
        return PLAYING_STATE;
    }

    return KEEP_STATE;
}
