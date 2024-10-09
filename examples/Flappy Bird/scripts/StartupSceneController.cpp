#include <NTTEngine/NTTEngine.hpp>
#include <iostream>

class StartupSceneController : public Script
{
public:
    StartupSceneController(void *data)
    {
    }

    ~StartupSceneController()
    {
    }

protected:
    void OnEnterImpl() override
    {
        m_timer.Reset();
        NTT_APP_INFO("Start the game");
    }

    void OnHover(HoveringContext &context) override
    {
    }

    void OnHoverEnter() override
    {
        GetComponent<Geometry>()->pos.x += 10;
    }

    void OnHoverExit() override
    {
        GetComponent<Geometry>()->pos.x -= 10;
    }

    void OnUpdateImpl(f32 deltaTime) override
    {
        if (m_timer.GetSeconds() > 1.0f)
        {
            SceneOpen("start");
        }
    }

private:
    Timer m_timer;
    b8 m_show = TRUE;
};

SCRIPT_DEFINE(StartupSceneController, Script)