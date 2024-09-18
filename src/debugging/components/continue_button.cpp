#include <NTTEngine/debugging/components/continue_button.hpp>
#include <NTTEngine/renderer/Hovering.hpp>
#include <NTTEngine/debugging/debugging.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>

namespace ntt
{
    using namespace renderer;
    using namespace input;
    using namespace debugging;

    class ContinueButton::Impl
    {
    public:
        void OnHover()
        {
            if (CheckState(NTT_BUTTON_LEFT, NTT_PRESS))
            {
                NextBreakpoint();
            }
        }
    };

    ContinueButton::ContinueButton()
    {
        m_impl = CreateScope<Impl>();
    }

    ContinueButton::~ContinueButton()
    {
    }

    void ContinueButton::OnEnterImpl()
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
