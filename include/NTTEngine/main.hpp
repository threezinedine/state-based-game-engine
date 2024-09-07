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

String GetSourceDir();

int main()
{
    LogInit();
    NTT_ENGINE_CONFIG(LogLevel::DEBUG, LOGGER_CONSOLE);
    ntt::Phrases phrases = {Begin, MainLoop, Close};

    ConfigureSourcePath(GetSourceDir());

    LoadConfiguration(RelativePath("configs/config.json"));
    auto config = GetConfiguration();

    ApplicationInit(
        config.Get<u16>("screenWidth", 800),
        config.Get<u16>("screenHeight", 600),
        config.Get<String>("title", "NTT Engine").RawString().c_str(),
        phrases);

    b8 running = true;

    while (running)
    {
        ApplicationUpdate(running);
    }

    ApplicationShutdown();

    LogShutdown();
    return 0;
}
#endif