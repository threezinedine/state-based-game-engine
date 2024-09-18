#include <NTTEngine/debugging/components/next_frame_button.hpp>
#include <NTTEngine/renderer/Hovering.hpp>
#include <NTTEngine/debugging/debugging.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>

namespace ntt
{
    using namespace renderer;
    using namespace input;
    using namespace debugging;

    class NextFrameButton::Impl
    {
    public:
        void OnHover()
        {
            if (CheckState(NTT_BUTTON_LEFT, NTT_PRESS))
            {
                NTT_ENGINE_DEBUG("Next frame button pressed");
                NextFrame();
            }
        }
    };

    NextFrameButton::NextFrameButton()
    {
        m_impl = CreateScope<Impl>();
    }

    NextFrameButton::~NextFrameButton()
    {
    }

    void NextFrameButton::OnEnterImpl()
    {
        GetComponent<Hovering>()->callback = [&](...)
        {
            m_impl->OnHover();
        };

        GetComponent<Hovering>()->onEnterCallback = [&](...)
        {
            GetComponent<Texture>()->colIndex = 5;
        };

        GetComponent<Hovering>()->onExitCallback = [&](...)
        {
            GetComponent<Texture>()->colIndex = 11;
        };
    }
} // namespace ntt
