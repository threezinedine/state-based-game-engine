#include <NTTEngine/NTTEngine.hpp>

#ifdef BUILD_GAME

void Begin();
void MainLoop(f32 delta);
void Close();

using namespace ntt;

void ConfigureStoredPath();
String GetSourceDir();
List<std::pair<String, SceneContext>> GetSceneFuncs();

int main()
{
    MemoryInit();
    LogInit();
    ProfilingInit("assets/profiling");
    EventInit();

    auto scenes = GetSceneFuncs();

    ntt::Phrases phrases = {
        [&scenes]()
        { Begin(); ECSBeginLayer(GAME_LAYER); SceneInit(scenes, ResourceChangeScene); },
        MainLoop,
        []()
        { SceneShutdown(); Close(); }};

    ConfigureSourcePath(GetSourceDir());
    ConfigureStoredPath();

    LoadConfiguration(RelativePath("assets/configs/config.json"));
    auto config = GetConfiguration();

    ProfilingBegin("Initialization");

    List<String> sceneNames = {};
    for (auto &scene : scenes)
    {
        sceneNames.push_back(scene.first);
    }

    ApplicationInit(
        config.Get<u16>("screenWidth", 800),
        config.Get<u16>("screenHeight", 600),
        config.Get<String>("title", "NTT Engine").RawString().c_str(),
        phrases,
        sceneNames,
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