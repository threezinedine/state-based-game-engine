#include "game_start.hpp"
#include "GameController.hpp"

void GameStart::OnEnterImpl()
{
    NTT_APP_INFO("Start Game");
    auto windowSize = GetWindowSize();

    m_messageEntity = ECSCreateEntity(
        "start-message",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, windowSize.height / 2, 200),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("message")),
        });

    ASSERT(m_messageEntity != INVALID_ENTITY_ID);
}

void GameStart::OnUpdateImpl(f32 delta)
{
}

void GameStart::OnExitImpl()
{
    ECSDeleteEntity(m_messageEntity);
}

String GameStart::OnNavigateImpl()
{
    if (CheckState(NTT_KEY_ENTER, NTT_DOWN))
    {
        return PLAYING_STATE;
    }

    return KEEP_STATE;
}
