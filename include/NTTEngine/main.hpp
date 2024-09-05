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

AdditionalData CreateAdditionalData();

int main()
{
    ntt::Phrases phrases = {Begin, MainLoop, Close};
    ApplicationInit(800, 600, "Hero", phrases, CreateAdditionalData());

    b8 running = true;

    while (running)
    {
        ApplicationUpdate(running);
    }

    ApplicationShutdown();

    return 0;
}
#endif