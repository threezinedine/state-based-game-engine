#include "GameController.hpp"
#include "PipeController.hpp"
#include "defs.hpp"

#define SPEEDUP_AFTER_DEFAULT 10

static f32 GetSpeed(u16 score)
{
    return 0.1f + static_cast<f32>(
                      score / SPEEDUP_AFTER_DEFAULT) *
                      0.01f;
}

void GameController::OnCreateImpl()
{
    GetData().Set<u16>("score", 0);
    GetData().Set<b8>("isPlaying", FALSE);

    Subscribe(
        NEW_PIPE_EVENT,
        [&](event_id_t id, void *sender, EventContext context)
        {
            auto posX = context.f32_data[0];
            AddNewPipe(posX);
        });

    Subscribe(
        ADD_SCORE,
        [&](event_id_t id, void *sender, EventContext context)
        {
            auto score = GetData().Get<u16>("score") + 1;
            GetData().Set<u16>("score", score);

            auto scoreChangedContext = EventContext();
            scoreChangedContext.u16_data[0] = score;
            TriggerEvent(SCORE_CHANGED_EVENT, this, scoreChangedContext);

            if (score % SPEEDUP_AFTER_DEFAULT == 0)
            {
                auto context = EventContext();
                context.f32_data[0] = GetSpeed(score);
                TriggerEvent(SPEED_UP_EVENT, this, context);
            }
        });

    Subscribe(
        GAME_OVER_EVENT,
        [&](...)
        {
            GetData().Set<b8>("isPlaying", FALSE);
        });

    Subscribe(
        PLAY_AGAIN_EVENT,
        [&](...)
        {
            GetData().Set<u16>("score", 0);
            TriggerEvent(SCORE_CHANGED_EVENT, this, {0});
            GetData().Set<b8>("isPlaying", TRUE);
            CreatePipe(GetWindowSize().width + 100, GetSpeed(0));
        });
}

void GameController::AddNewPipe(position_t posX)
{
    CreatePipe(posX, GetSpeed(GetData().Get<u16>("score")));
}
