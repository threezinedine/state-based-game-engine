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
List<std::pair<String, std::function<void()>>> GetSceneFuncs();

int main()
{
    LogInit();
    EventInit();
    NTT_ENGINE_CONFIG(LogLevel::DEBUG, LOGGER_CONSOLE);
    ntt::Phrases phrases = {
        []()
        { Begin(); BeginLayer(GAME_LAYER) ;SceneInit(GetSceneFuncs()); },
        MainLoop,
        []()
        { SceneShutdown(); Close(); }};

    ConfigureSourcePath(GetSourceDir());
    ProfilingInit("profiling");

    LoadConfiguration(RelativePath("configs/config.json"));
    auto config = GetConfiguration();

    ProfilingBegin("Initialization");

    ApplicationInit(
        config.Get<u16>("screenWidth", 800),
        config.Get<u16>("screenHeight", 600),
        config.Get<String>("title", "NTT Engine").RawString().c_str(),
        phrases);

    b8 running = true;

    ProfilingBegin("Loop");

    while (running)
    {
        ApplicationUpdate(running);
    }

    ProfilingBegin("Shutdown");
    ApplicationShutdown();

    ProfilingShutdown();
    EventShutdown();
    LogShutdown();
    return 0;
}
#endif