#include "game_start.hpp"
#include "game_data.hpp"

void GameStart::OnEnterImpl()
{
    auto windowSize = GetWindowSize();

    m_messageEntity = ECSCreateEntity(
        "Message",
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
    if (GetGameData()->state == GameState::PLAYING)
    {
        return PLAYING_STATE;
    }

    return KEEP_STATE;
}
