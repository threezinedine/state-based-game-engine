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

void ConfigureStoredPath();
String GetSourceDir();
List<std::pair<String, SceneContext>> GetSceneFuncs();

int main()
{
    MemoryInit();
    LogInit();
    ProfilingInit("assets/profiling");
    EventInit();
    ntt::Phrases phrases = {
        []()
        { Begin(); ECSBeginLayer(GAME_LAYER) ;SceneInit(GetSceneFuncs(), ResourceChangeScene); },
        MainLoop,
        []()
        { SceneShutdown(); Close(); }};

    ConfigureSourcePath(GetSourceDir());
    ConfigureStoredPath();

    LoadConfiguration(RelativePath("assets/configs/config.json"));
    auto config = GetConfiguration();

    ProfilingBegin("Initialization");

    ApplicationInit(
        config.Get<u16>("screenWidth", 800),
        config.Get<u16>("screenHeight", 600),
        config.Get<String>("title", "NTT Engine").RawString().c_str(),
        phrases,
#ifdef _EDITOR
        TRUE
#else
        FALSE
#endif
    );

    b8 running = true;

    ProfilingBegin("Loop");

    while (running)
    {
        ApplicationUpdate(running);
    }

    ProfilingBegin("Shutdown");
    ApplicationShutdown();

    EventShutdown();
    ProfilingShutdown();
    LogShutdown();
    MemoryShutdown();
    return 0;
}
#endif