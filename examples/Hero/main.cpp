#include <NTTEngine/NTTEngine.hpp>

using namespace ntt;
using namespace ntt::log;
using namespace ntt::input;
using namespace ntt::event;

int main()
{
    NTT_APP_CONFIG(LogLevel::INFO, LOGGER_CONSOLE, "[@l]:[@n] - @t - @f:@L: @m");
    NTT_APP_INFO("Starting the game");
    auto game = ntt::CreateApplication(800, 600, "Hero");

    NTT_APP_INFO("The game started with the size: (%d, %d)", 800, 600);

    auto id = RegisterEvent(EventCode::KEY_PRESSED,
                            [](EventCode event_code, void *sender, const EventContext &context)
                            { NTT_APP_INFO("Button \"%s\" is pressed",
                                           GetKeyName(static_cast<Key>(context.u16_data[0]))); });

    RegisterEvent(EventCode::WINDOW_RESIZED,
                  [](EventCode event_code, void *sender, const EventContext &context)
                  { NTT_APP_INFO("Window is resized to (%d, %d)",
                                 context.u32_data[0], context.u32_data[1]); });

    game->Begin();

    while (!game->ShouldClose())
    {
        game->Update();

        if (CheckState(Key::NTT_KEY_A, InputState::NTT_PRESS))
        {
            UnregisterEvent(id);
        }
    }

    game->End();

    return 0;
}