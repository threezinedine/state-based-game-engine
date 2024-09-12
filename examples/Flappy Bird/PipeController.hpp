#pragma once

#include <NTTEngine/NTTEngine.hpp>

class PipeController : public Script
{
public:
    PipeController();

    void OnEnterImpl() override;
    void OnUpdateImpl(f32 delta) override;
    void OnExitImpl() override;

private:
    position_t m_getScoreX;
    b8 m_hasAfter = FALSE;
    b8 m_isPassed = FALSE;
};

void CreatePipe(position_t posX, f32 speed);