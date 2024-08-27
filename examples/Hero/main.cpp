#include <NTTEngine/NTTEngine.hpp>

using namespace ntt;
using namespace ntt::log;

int main()
{
    NTT_APP_INFO("Starting the game");
    auto game = ntt::CreateApplication(800, 600, "Hero");

    NTT_APP_INFO("The game started with the size: (%d, %d)", 800, 600);

    game->Begin();

    while (!game->ShouldClose())
    {
        game->Update(0.0f);
    }

    game->End();

    return 0;
}