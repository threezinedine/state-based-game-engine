#include "defs.hpp"
#include <NTTEngine/NTTEngine.hpp>

#define SPEEDUP_AFTER_DEFAULT 10

static f32 GetSpeed(u16 score)
{
    return 0.1f + static_cast<f32>(
                      score / SPEEDUP_AFTER_DEFAULT) *
                      0.01f;
}

class GameController : public Script
{
public:
    GameController(void *data)
    {
        PROFILE_FUNCTION();
    }

    ~GameController()
    {
        PROFILE_FUNCTION();
    }

protected:
    void OnEnterImpl() override
    {
        PROFILE_FUNCTION();
        GetData().Set<u16>("score", 0);
        GetData().Set<b8>("isPlaying", FALSE);

        Subscribe(NEW_PIPE_EVENT);
        Subscribe(ADD_SCORE);
        Subscribe(GAME_OVER_EVENT);
        Subscribe(PLAY_AGAIN_EVENT);
    }

    void OnEvent(event_code_t code, void *sender, const EventContext &context) override
    {
        PROFILE_FUNCTION();
        switch (code)
        {
        case NEW_PIPE_EVENT:
            AddNewPipe(context.f32_data[0]);
            break;
        case ADD_SCORE:
            AddScore();
            break;
        case GAME_OVER_EVENT:
            GetData().Set<b8>("isPlaying", FALSE);
            break;

        case PLAY_AGAIN_EVENT:
            PlayAgain();
            break;
        }
    }

    void PlayAgain()
    {
        PROFILE_FUNCTION();
        GetData().Set<u16>("score", 0);
        TriggerEvent(SCORE_CHANGED_EVENT, this, {0});
        GetData().Set<b8>("isPlaying", TRUE);
        AddNewPipe(GetWindowSize().width + 100);
    }

    void AddScore()
    {
        PROFILE_FUNCTION();
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
    }

    void AddNewPipe(position_t posX)
    {
        PROFILE_FUNCTION();
        auto speed = GetSpeed(GetData().Get<u16>("score"));
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
            "pipe-up",
            {
                ECS_CREATE_COMPONENT(Geometry, posX, highPipeY, 100, highPipeY * 2, 180),
                ECS_CREATE_COMPONENT(Texture, GetResourceID("pipe")),
                ECS_CREATE_COMPONENT(Mass, 1.0f, -speed, 0, 0, 0),
                ECS_CREATE_COMPONENT(Collision),
                ECS_CREATE_COMPONENT(NativeScriptComponent,
                                     GetResourceID("pipe-controller")),
            });

        auto downPipe = ECSCreateEntity(
            "pipe-down",
            {
                ECS_CREATE_COMPONENT(Geometry, posX, lowPipeY, 100, lowPipeHeight),
                ECS_CREATE_COMPONENT(Texture, GetResourceID("pipe")),
                ECS_CREATE_COMPONENT(Mass, 1.0f, -speed, 0, 0, 0),
                ECS_CREATE_COMPONENT(Collision),
                ECS_CREATE_COMPONENT(NativeScriptComponent,
                                     GetResourceID("pipe-controller")),
            });
    }

private:
};

SCRIPT_DEFINE(GameController)