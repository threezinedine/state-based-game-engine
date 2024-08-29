#include <NTTEngine/NTTEngine.hpp>

using namespace ntt;
using namespace ntt::log;
using namespace ntt::input;

int main()
{
    NTT_APP_CONFIG(LogLevel::INFO, LOGGER_CONSOLE, "[@l]:[@n] - @t - @f:@L: @m");
    NTT_APP_INFO("Starting the game");
    auto game = ntt::CreateApplication(800, 600, "Hero");

    NTT_APP_INFO("The game started with the size: (%d, %d)", 800, 600);

    game->Begin();

    while (!game->ShouldClose())
    {
        game->Update();

        if (CheckState(Key::NTT_KEY_A, InputState::NTT_PRESS))
        {
            NTT_APP_INFO("Key A is pressed");
        }
    }

    game->End();

    return 0;
}