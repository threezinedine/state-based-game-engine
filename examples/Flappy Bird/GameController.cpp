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
            GetComponent<GameData>()->score++;

            TriggerEvent(SCORE_CHANGED_EVENT, this, {GetComponent<GameData>()->score});

            if (GetComponent<GameData>()->score % SPEEDUP_AFTER_DEFAULT == 0)
            {
                auto context = EventContext();
                context.f32_data[0] = GetSpeed(GetComponent<GameData>()->score);
                TriggerEvent(SPEED_UP_EVENT, this, context);
            }
        });

    Subscribe(
        GAME_OVER_EVENT,
        [&](...)
        {
            GetComponent<GameData>()->isPlaying = FALSE;
        });

    Subscribe(
        PLAY_AGAIN_EVENT,
        [&](...)
        {
            GetComponent<GameData>()->score = 0;
            GetComponent<GameData>()->isPlaying = TRUE;
            CreatePipe(GetWindowSize().width + 100, GetSpeed(0));
        });
}

void GameController::AddNewPipe(position_t posX)
{
    CreatePipe(posX, GetSpeed(GetComponent<GameData>()->score));
}
