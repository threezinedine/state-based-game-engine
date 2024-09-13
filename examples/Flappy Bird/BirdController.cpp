#include "BirdController.hpp"
#include "defs.hpp"
#include <algorithm>

void BirdController::OnEnterImpl()
{
    GetComponent<Collision>()->callback =
        std::bind(&BirdController::OnCollide, this, std::placeholders::_1);

    GetComponent<Hovering>()->callback =
        std::bind(&BirdController::OnHover, this, std::placeholders::_1);

    StopBird();

    Subscribe(
        PLAY_AGAIN_EVENT,
        std::bind(&BirdController::StartBird, this));

    Subscribe(
        GAME_OVER_EVENT,
        std::bind(&BirdController::StopBird, this));

    GetComponent<Mass>()->AddForceConst(0, 0.001);
    SetComponentState<Geometry>(FALSE);
}

void BirdController::OnUpdateImpl(f32 delta)
{
    if (m_isPlaying)
    {
        if (CheckState(NTT_KEY_SPACE, NTT_PRESS))
        {
            GetComponent<Mass>()->velocity_y = -0.3;
            PlayAudio(GetResourceID("wing"));
        }

        auto geo = GetComponent<Geometry>();
        auto velocityY = GetComponent<Mass>()->velocity_y;
        auto rotation = geo->rotatation;

        if (velocityY > 0)
        {
            rotation += 2;
        }
        else
        {
            rotation -= 2;
        }

        rotation = std::max<int>(-30, std::min<int>(30, rotation));
        geo->rotatation = rotation;
    }
}

void BirdController::StopBird()
{
    SetComponentState<Mass>(FALSE);
    SetComponentState<Sprite>(FALSE);
    m_isPlaying = FALSE;
}

void BirdController::StartBird()
{
    SetComponentState<Geometry>(TRUE);
    SetComponentState<Mass>(TRUE);
    SetComponentState<Sprite>(TRUE);
    GetComponent<Geometry>()->y = GetWindowSize().height / 2;
    GetComponent<Mass>()->velocity_y = 0;
    m_isPlaying = TRUE;
}

void BirdController::OnCollide(List<entity_id_t> others)
{
    PlayAudio(GetResourceID("hit"));
    TriggerEvent(GAME_OVER_EVENT);
}