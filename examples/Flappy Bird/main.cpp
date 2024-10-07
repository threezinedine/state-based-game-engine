#include <NTTEngine/main.hpp>
#include "defs.hpp"

String GetSourceDir()
{
    return {"C:/Users/Acer/Games Dev/state-based-game-engine/examples/Flappy Bird"};
}

void ConfigureStoredPath()
{
    SetSetPath(PathType::NTT_BINARY, "C:/Users/Acer/Games Dev/state-based-game-engine/build");
    SetSetPath(PathType::NTT_ENGINE, "C:/Users/Acer/Games Dev/state-based-game-engine");
    SetSetPath(PathType::NTT_SOURCE, "C:/Users/Acer/Games Dev/state-based-game-engine/examples/Flappy Bird");
}

void CreateStartupScene();
void CreateScene1();

List<std::pair<String, SceneContext>> GetSceneFuncs()
{
    return {
        {"startup", {CreateStartupScene, nullptr}},
        {"game", {CreateScene1, nullptr}},
    };
}

void CreateStartupScene()
{
    auto windowSize = GetWindowSize();

    ECSCreateEntity(
        "startup",
        {
            ECS_CREATE_COMPONENT(Geometry,
                                 windowSize.width / 2,
                                 windowSize.height / 2,
                                 windowSize.width * 2 / 3),
            ECS_CREATE_COMPONENT(TextureComponent, GetResourceID("startup")),
            ECS_CREATE_COMPONENT(NativeScriptComponent, GetResourceID("startup-scene-controller")),
            ECS_CREATE_COMPONENT(Hovering),
            ECS_CREATE_COMPONENT(Mass, 1.0f),
        });
}

void CreateScene1()
{
    auto windowSize = GetWindowSize();
    ECSCreateEntity(
        "background",
        {
            ECS_CREATE_COMPONENT(Geometry,
                                 windowSize.width / 2,
                                 windowSize.height / 2,
                                 windowSize.width,
                                 windowSize.height),
            ECS_CREATE_COMPONENT(TextureComponent, GetResourceID("background")),
        });

    ECSCreateEntity(
        "menu-btn",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width * 0.95, windowSize.height * 0.05, 30),
            ECS_CREATE_COMPONENT(TextureComponent, GetResourceID("buttons"),
                                 9, 11,
                                 PRIORITY_2,
                                 "Setting"),
            ECS_CREATE_COMPONENT(NativeScriptComponent, GetResourceID("setting-btn-controller")),
            ECS_CREATE_COMPONENT(Hovering),
        });

    ECSCreateEntity(
        "base-high",
        {
            ECS_CREATE_COMPONENT(Geometry,
                                 windowSize.width / 2,
                                 windowSize.height / 20,
                                 windowSize.width,
                                 windowSize.height / 10,
                                 180),
            ECS_CREATE_COMPONENT(TextureComponent, GetResourceID("base"), 0, 0, PRIORITY_1),
            ECS_CREATE_COMPONENT(Collision),
        });

    ECSCreateEntity(
        "base-low",
        {
            ECS_CREATE_COMPONENT(Geometry,
                                 windowSize.width / 2,
                                 windowSize.height / 20 * 19,
                                 windowSize.width,
                                 windowSize.height / 10),
            ECS_CREATE_COMPONENT(TextureComponent, GetResourceID("base"), 0, 0, PRIORITY_1),
            ECS_CREATE_COMPONENT(Collision),
        });

    ECSCreateEntity(
        "game-play",
        {
            ECS_CREATE_COMPONENT(NativeScriptComponent, GetResourceID("game-controller")),
            ECS_CREATE_COMPONENT(
                StateComponent,
                Dictionary<String, resource_id_t>{
                    {START_STATE, GetResourceID("game-fsm-start")},
                    {GAME_OVER_STATE, GetResourceID("game-fsm-over")},
                    {PLAYING_STATE, GetResourceID("game-fsm-playing")},
                },
                START_STATE),
        });

    auto bird = ECSCreateEntity(
        "bird",
        {
            ECS_CREATE_COMPONENT(Geometry, GetConfiguration().Get<position_t>("bird-start-x", 200),
                                 windowSize.height / 2, 70),
            ECS_CREATE_COMPONENT(TextureComponent, GetResourceID("bird"), 0, 0, PRIORITY_2,
                                 "The bird"),
            ECS_CREATE_COMPONENT(Mass, 1.0f),
            ECS_CREATE_COMPONENT(Collision),
            ECS_CREATE_COMPONENT(Sprite,
                                 List<std::pair<u8, u8>>{{0, 0}, {1, 0}, {2, 0}},
                                 200),
            ECS_CREATE_COMPONENT(Hovering),
            ECS_CREATE_COMPONENT(NativeScriptComponent, GetResourceID("bird-controller")),
        });

    ECSCreateEntity(
        "score-board",
        {
            ECS_CREATE_COMPONENT(Geometry, 20, 20),
            ECS_CREATE_COMPONENT(NativeScriptComponent, GetResourceID("score-board")),
        });

    ECSCreateEntity(
        "hello world text",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, windowSize.height / 2),
            ECS_CREATE_COMPONENT(Text, "Hello World", 20, PRIORITY_0, NTT_CYAN),
        });

    RegisterEvent(
        SPEED_UP_EVENT,
        [](auto id, void *sender, EventContext context)
        {
            auto speed = context.f32_data[0];
            NTT_APP_INFO("New Speed {} for all pipes", speed);
        });
}

void Begin()
{
    NTT_APP_CONFIG(
        LogLevel::DEBUG,
        {CreateRef<ConsoleHandler>(),
         CreateRef<EditorLogHandler>()});

    auto windowSize = GetWindowSize();

    ECSBeginLayer(UI_LAYER_0);
    ECSCreateEntity(
        "resume-btn",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, windowSize.height / 2, 100),
            ECS_CREATE_COMPONENT(TextureComponent, GetResourceID("menu-btn"), 1, 1,
                                 PRIORITY_0,
                                 "Resume"),
            ECS_CREATE_COMPONENT(Hovering),
            ECS_CREATE_COMPONENT(NativeScriptComponent, GetResourceID("resume-btn-controller")),
        });

    RegisterEvent(
        NTT_LAYER_CHANGED,
        [](auto id, void *sender, EventContext context)
        {
            auto layer = context.u16_data[0];
            NTT_APP_DEBUG("Layer {} is changed", layer);
        });

    RegisterEvent(
        NTT_SCENE_CHANGED,
        [](auto id, void *sender, EventContext context)
        {
            NTT_APP_DEBUG("Scene is changed");
        });
}

void MainLoop(f32 delta)
{
    if (CHECK_PRESS(NTT_KEY_R))
    {
        SceneReload();
    }

    if (CHECK_PRESS(NTT_KEY_P))
    {
        NTT_APP_INFO("P is pressed");
    }

    if (CHECK_PRESS(NTT_KEY_D))
    {
        EditorRun();
    }

    if (CHECK_PRESS(NTT_KEY_S))
    {
        EditorStop();
    }

    if (CHECK_PRESS(NTT_KEY_ONE))
    {
        SceneOpen("startup");
    }

    if (CHECK_PRESS(NTT_KEY_TWO))
    {
        SceneOpen("game");
    }
}

void Close()
{
}