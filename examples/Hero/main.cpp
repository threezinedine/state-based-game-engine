#include <NTTEngine/NTTEngine.hpp>

int main()
{
    auto game = ntt::CreateApplication(800, 600, "Hero");

    game->Begin();

    while (!game->ShouldClose())
    {
        game->Update(0.0f);
    }

    game->End();

    return 0;
}