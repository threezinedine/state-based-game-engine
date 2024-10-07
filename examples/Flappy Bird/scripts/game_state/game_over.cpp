#include "defs.hpp"
#include <NTTEngine/NTTEngine.hpp>

class GameOver : public State
{
public:
    GameOver(void *data)
    {
    }

    ~GameOver()
    {
    }

protected:
    void OnEnterImpl() override
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
                    ECS_CREATE_COMPONENT(TextureComponent, GetResourceID("gameover"), 0, 0, PRIORITY_4),
                });
        }

        ECSSetComponentActive(m_gameOverMessage, typeid(Geometry), TRUE);
    }

    String OnNavigateImpl() override
    {
        if (CheckState(NTT_KEY_ENTER, NTT_DOWN))
        {
            return PLAYING_STATE;
        }

        return KEEP_STATE;
    }

    void OnUpdateImpl(f32 delta) override {}

    void OnExitImpl() override
    {
        ECSSetComponentActive(m_gameOverMessage, typeid(Geometry), FALSE);
    }

private:
    entity_id_t m_gameOverMessage = INVALID_ENTITY_ID;
};

SCRIPT_DEFINE(GameOver, State)