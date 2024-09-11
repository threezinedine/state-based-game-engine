#include "PipeController.hpp"
#include "defs.hpp"

#define SPEEDUP_AFTER_DEFAULT 10

namespace
{
}

#define variable (m_impl->variable)

PipeController::PipeController()
{
    m_getScoreX = GetConfiguration().Get<position_t>("bird-start-x", 200);
}

void PipeController::OnUpdateImpl(f32 delta)
{
    auto geo = GetComponent<Geometry>();
    auto config = GetConfiguration();

    if (geo->rotatation == 0)
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

void PipeController::OnCreateImpl()
{
    Subscribe(
        GAME_OVER_EVENT,
        [&](...)
        {
            SetComponentState<Mass>(FALSE);
        });

    Subscribe(
        PLAY_AGAIN_EVENT,
        [&](...)
        { Delete(); });

    Subscribe(
        SPEED_UP_EVENT,
        [&](auto id, auto sender, EventContext context)
        {
            auto speed = context.f32_data[0];
            GetComponent<Mass>()->velocity_x = -speed;
        });
}

void PipeController::OnDestroyImpl()
{
}

void CreatePipe(position_t posX, f32 speed)
{
    auto windowSize = GetWindowSize();
    auto config = GetConfiguration();

    auto getScoreX = config.Get<position_t>("bird-x", 200);

    auto posYStart = windowSize.height * config.Get<f32>("pipe-start-range", 3.0f / 4);
    auto posYEnd = windowSize.height * config.Get<f32>("pipe-end-range", 7.0f / 8);
    auto posGapY = Random(posYStart, posYEnd);
    auto gap = config.Get<f32>("pipe-gap", 100);

    auto highPipeY = (posGapY - gap / 2) / 2;

    auto lowPipeHeight = windowSize.height - posGapY - gap / 2;
    auto lowPipeY = windowSize.height - lowPipeHeight / 2;

    auto upPipe = ECSCreateEntity(
        {
            ECS_CREATE_COMPONENT(Geometry, posX, highPipeY, 100, highPipeY * 2, 180),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("pipe")),
            ECS_CREATE_COMPONENT(Mass, 1.0f, -speed, 0, 0, 0),
            ECS_CREATE_COMPONENT(Collision),
            ECS_CREATE_COMPONENT(NativeScriptComponent,
                                 CreateRef<PipeController>()),
        });

    auto downPipe = ECSCreateEntity(
        {
            ECS_CREATE_COMPONENT(Geometry, posX, lowPipeY, 100, lowPipeHeight),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("pipe")),
            ECS_CREATE_COMPONENT(Mass, 1.0f, -speed, 0, 0, 0),
            ECS_CREATE_COMPONENT(Collision),
            ECS_CREATE_COMPONENT(NativeScriptComponent,
                                 CreateRef<PipeController>()),
        });
}