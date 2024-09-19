#pragma once

#include <NTTEngine/NTTEngine.hpp>

class StartupSceneController : public Script
{
protected:
    void OnEnterImpl() override
    {
        m_timer.Reset();
    }

    void OnUpdateImpl(f32 deltaTime) override
    {
        if (m_timer.GetSeconds() > 3.0f)
        {
            SceneOpen("game");
        }
    }

private:
    Timer m_timer;
};