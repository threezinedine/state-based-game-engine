#include "PipeController.hpp"
#include "game_data.hpp"

#define SPEEDUP_AFTER_DEFAULT 10

class PipeController : public Script
{
public:
    PipeController();

    void OnUpdate(f32 delta) override;

private:
    position_t m_getScoreX;
    b8 m_hasAfter = FALSE;
    b8 m_isPassed = FALSE;
};

namespace
{
    entity_id_t s_createdPipeCount = 0;
    List<entity_id_t> s_pipes;
    f32 s_pipeSpeed = 0.1;

    f32 GetPipeSpeed(u16 score)
    {
        auto config = GetConfiguration();
        return config.Get<f32>("pipe-speed", 0.1) +
               static_cast<u8>(score / config.Get<u8>("speedup-after", SPEEDUP_AFTER_DEFAULT)) *
                   config.Get<f32>("pipe-increase", 0.01);
    }

    void UpdatePipeSpeed(u16 score)
    {
        s_pipeSpeed = GetPipeSpeed(score);
        for (auto pipe : s_pipes)
        {
            auto mass = ECS_GET_COMPONENT(pipe, Mass);
            mass->velocity_x = -s_pipeSpeed;
        }
    }

}

#define variable (m_impl->variable)

PipeController::PipeController()
{
    m_getScoreX = GetConfiguration().Get<position_t>("bird-x", 200);
}

void PipeController::OnUpdate(f32 delta)
{
    auto geo = GetComponent<Geometry>();
    auto config = GetConfiguration();

    if (geo->rotatation == 0)
    {
        if (geo->x < m_getScoreX && m_isPassed == FALSE)
        {
            m_isPassed = TRUE;
            GetGameData()->score++;
            audio::PlayAudio(GetResourceID("point"));
        }

        if (geo->x < GetWindowSize().width && m_hasAfter == FALSE)
        {
            m_hasAfter = TRUE;

            auto pos = GetWindowSize().width + config.Get<position_t>("pipe-distance", 300);

            CreatePipe(pos);
        }
    }

    if (geo->x + geo->width < 0)
    {
        s_pipes.RemoveItem(GetEntity());
        ECSDeleteEntity(GetEntity());
    }
}

void CreatePipe(position_t posX)
{
    auto pipeNameUp = String("Pipe %d up", s_createdPipeCount);
    auto pipeNameDown = String("Pipe %d down", s_createdPipeCount++);
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
        pipeNameUp,
        {
            ECS_CREATE_COMPONENT(Geometry, posX, highPipeY, 100, highPipeY * 2, 180),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("pipe")),
            ECS_CREATE_COMPONENT(Mass, 1.0f, -s_pipeSpeed, 0, 0, 0),
            ECS_CREATE_COMPONENT(Collision),
            ECS_CREATE_COMPONENT(NativeScriptComponent,
                                 CreateRef<PipeController>()),
        });

    auto downPipe = ECSCreateEntity(
        pipeNameDown,
        {
            ECS_CREATE_COMPONENT(Geometry, posX, lowPipeY, 100, lowPipeHeight),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("pipe")),
            ECS_CREATE_COMPONENT(Mass, 1.0f, -s_pipeSpeed, 0, 0, 0),
            ECS_CREATE_COMPONENT(Collision),
            ECS_CREATE_COMPONENT(NativeScriptComponent,
                                 CreateRef<PipeController>()),
        });

    s_pipes.push_back(upPipe);
    s_pipes.push_back(downPipe);
}

void ResetPipe()
{
    for (auto pipe : s_pipes)
    {
        ECSDeleteEntity(pipe);
    }

    s_pipes.clear();
    s_createdPipeCount = 0;
}

void StopPipe()
{
    for (auto pipe : s_pipes)
    {
        ECSSetComponentActive(pipe, typeid(Mass), FALSE);
        ECSSetComponentActive(pipe, typeid(Collision), FALSE);
    }
}