#include "defs.hpp"
#include <NTTEngine/NTTEngine.hpp>

class GameStart : public State
{
public:
    GameStart(void *data)
    {
    }

    ~GameStart()
    {
    }

protected:
    void OnEnterImpl() override
    {
        NTT_APP_INFO("Start Game");
        auto windowSize = GetWindowSize();

        m_messageEntity = ECSCreateEntity(
            "start-message",
            {
                ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, windowSize.height / 2, 200),
                ECS_CREATE_COMPONENT(TextureComponent, GetResourceID("message")),
            });

        ASSERT(m_messageEntity != INVALID_ENTITY_ID);

        PlayAudio(GetResourceID("background-audio"), {0});
    }

    String OnNavigateImpl() override
    {
        if (CheckState(NTT_KEY_ENTER, NTT_DOWN))
        {
            return PLAYING_STATE;
        }

        return KEEP_STATE;
    }

    void OnUpdateImpl(f32 delta) override
    {
    }

    void OnExitImpl() override
    {
        ECSDeleteEntity(m_messageEntity);
    }

private:
    entity_id_t m_messageEntity = INVALID_ENTITY_ID;
};

SCRIPT_DEFINE(GameStart, State)