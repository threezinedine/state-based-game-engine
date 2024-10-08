#include "defs.hpp"
#include <algorithm>

#include <NTTEngine/NTTEngine.hpp>

class BirdController : public Script
{
public:
    BirdController(void *data)
    {
    }

    ~BirdController()
    {
    }

protected:
    void OnCollide(List<entity_id_t> others) override
    {
        PROFILE_FUNCTION();
        PlayAudio(GetResourceID("hit"));
        TriggerEvent(GAME_OVER_EVENT);
    }

    void OnEvent(
        event_code_t eventCode,
        void *sender,
        const EventContext &context) override
    {
        PROFILE_FUNCTION();
        switch (eventCode)
        {
        case PLAY_AGAIN_EVENT:
            StartBird();
            break;
        case GAME_OVER_EVENT:
            StopBird();
            break;
        }
    }

    void StopBird()
    {
        PROFILE_FUNCTION();
        SetComponentState<Mass>(FALSE);
        SetComponentState<Sprite>(FALSE);
        m_isPlaying = FALSE;
    }

    void StartBird()
    {
        PROFILE_FUNCTION();
        SetComponentState<Geometry>(TRUE);
        SetComponentState<Mass>(TRUE);
        SetComponentState<Sprite>(TRUE);
        GetComponent<Geometry>()->y = GetWindowSize().height / 2;
        GetComponent<Mass>()->velocity_y = 0;
        m_isPlaying = TRUE;
    }

    void OnHover(HoveringContext &context) override
    {
        PROFILE_FUNCTION();
        NTT_APP_DEBUG("Hovering");
    }

    void OnEnterImpl() override
    {
        PROFILE_FUNCTION();
        StopBird();
        Subscribe(PLAY_AGAIN_EVENT);
        Subscribe(GAME_OVER_EVENT);

        ASSERT_M(GetComponent<Mass>() != nullptr, "Mass component is not found!");
        GetComponent<Mass>()->AddForceConst(0, 0.001);

        TriggerEvent(PLAY_AGAIN_EVENT);
    }

    void OnUpdateImpl(f32 delta) override
    {
        PROFILE_FUNCTION();
        if (m_isPlaying)
        {
            if (CheckState(NTT_KEY_SPACE, NTT_PRESS))
            {
                GetComponent<Mass>()->velocity_y = -0.3;
                PlayAudio(GetResourceID("wing"));
            }

            auto geo = GetComponent<Geometry>();
            auto velocityY = GetComponent<Mass>()->velocity_y;
            auto rotation = geo->rotation;

            if (velocityY > 0)
            {
                rotation += 2;
            }
            else
            {
                rotation -= 2;
            }

            rotation = std::max<int>(-30, std::min<int>(30, rotation));
            geo->rotation = rotation;
        }
    }

    void OnExitImpl() override
    {
        PROFILE_FUNCTION();
    }

private:
    b8 m_isPlaying = FALSE;
};

SCRIPT_DEFINE(BirdController, Script)