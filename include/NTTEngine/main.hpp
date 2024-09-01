#include <NTTEngine/NTTEngine.hpp>

#ifdef BUILD_GAME

void Begin();
void MainLoop(f32 delta);
void Close();

using namespace ntt;
using namespace ntt::log;
using namespace ntt::input;
using namespace ntt::event;
using namespace ntt::renderer;
using namespace ntt::memory;

int main()
{
    ntt::Phrases phrases = {Begin, MainLoop, Close};
    auto game = ntt::CreateApplication(800, 600, "Hero", phrases);

    game->Begin();

    while (!game->ShouldClose())
    {
        game->Update();
    }

    game->End();

    return 0;
}
#endif