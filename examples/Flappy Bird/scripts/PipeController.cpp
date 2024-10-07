#include "defs.hpp"
#include <NTTEngine/NTTEngine.hpp>

#define SPEEDUP_AFTER_DEFAULT 10

namespace
{
}

#define variable (m_impl->variable)

class PipeController : public Script
{
public:
    PipeController(void *)
    {
        m_getScoreX = GetConfiguration().Get<position_t>("bird-start-x", 200);
    }

    ~PipeController() {}

    void OnEnterImpl() override
    {
        Subscribe(GAME_OVER_EVENT);
        Subscribe(PLAY_AGAIN_EVENT);
        Subscribe(SPEED_UP_EVENT);
    }

    void OnEvent(event_code_t code, void *sender, const EventContext &context) override
    {
        switch (code)
        {
        case GAME_OVER_EVENT:
            SetComponentState<Mass>(FALSE);
            break;
        case PLAY_AGAIN_EVENT:
            Delete();
            break;
        case SPEED_UP_EVENT:
            GetComponent<Mass>()->velocity_x = -context.f32_data[0];
            break;
        }
    }

    void OnUpdateImpl(f32 delta) override
    {
        auto geo = GetComponent<Geometry>();
        auto config = GetConfiguration();

        if (geo->rotation == 0)
        {
            if (geo->x < m_getScoreX && m_isPassed == FALSE)
            {
                m_isPassed = TRUE;
                TriggerEvent(ADD_SCORE);
                audio::PlayAudio(GetResourceID("point"));
            }

            if (geo->x < GetWindowSize().width && m_hasAfter == FALSE)
            {
                m_hasAfter = TRUE;
                auto context = EventContext();
                context.f32_data[0] = GetComponent<Geometry>()->x + config.Get<position_t>("pipe-distance", 200);
                TriggerEvent(NEW_PIPE_EVENT, this, context);
            }
        }

        if (geo->x + geo->width < 0)
        {
            Delete();
        }
    }

    void OnExitImpl() override {}

private:
    position_t m_getScoreX;
    b8 m_hasAfter = FALSE;
    b8 m_isPassed = FALSE;
};

SCRIPT_DEFINE(PipeController, Script)