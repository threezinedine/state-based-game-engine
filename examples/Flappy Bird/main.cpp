#include <NTTEngine/main.hpp>
#include "PipeController.hpp"
#include "game_state/game_state.hpp"
#include "ScoreBoard.hpp"
#include "BirdController.hpp"
#include "GameController.hpp"
#include "defs.hpp"
#include "SettingButtonController.hpp"
#include "ResumeButtonController.hpp"
#include "StartupSceneController.hpp"

String GetSourceDir()
{
    return {"C:/Users/Acer/Games Dev/state-based-game-engine/examples/Flappy Bird/assets"};
}

void CreateStartupScene();
void CreateScene1();

List<std::pair<String, std::function<void()>>> GetSceneFuncs()
{
    return {
        {"startup", CreateStartupScene},
        {"game", CreateScene1},
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
            ECS_CREATE_COMPONENT(Texture, GetResourceID("startup")),
            ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<StartupSceneController>()),
        });

    RegisterEvent(
        NTT_RESOURCE_LOADED,
        [](auto id, void *sender, EventContext context)
        {
            auto resourceId = context.u32_data[0];
            NTT_APP_INFO("Resource {} is loaded", resourceId);
        });

    NTT_ENGINE_INFO("The startup scene is created");
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
            ECS_CREATE_COMPONENT(Texture, GetResourceID("background")),
        });

    ECSCreateEntity(
        "menu-btn",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width * 0.95, windowSize.height * 0.05, 30),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("buttons"),
                                 9, 11,
                                 PRIORITY_2,
                                 "Setting"),
            ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<SettingButtonController>()),
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
            ECS_CREATE_COMPONENT(Texture, GetResourceID("base"), 0, 0, PRIORITY_1),
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
            ECS_CREATE_COMPONENT(Texture, GetResourceID("base"), 0, 0, PRIORITY_1),
            ECS_CREATE_COMPONENT(Collision),
        });

    auto gameState = CreateRef<State>();
    gameState->AddChild(START_STATE, CreateRef<GameStart>());
    gameState->AddChild(PLAYING_STATE, CreateRef<GamePlaying>());
    gameState->AddChild(GAME_OVER_STATE, CreateRef<GameOver>());

    ECSCreateEntity(
        "game-play",
        {
            ECS_CREATE_COMPONENT(StateComponent, gameState),
            ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<GameController>()),
        });

    auto bird = ECSCreateEntity(
        "bird",
        {
            ECS_CREATE_COMPONENT(Geometry, GetConfiguration().Get<position_t>("bird-start-x", 200),
                                 windowSize.height / 2, 70),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("bird"), 0, 0, PRIORITY_2,
                                 "The bird"),
            ECS_CREATE_COMPONENT(Mass, 1.0f),
            ECS_CREATE_COMPONENT(Collision),
            ECS_CREATE_COMPONENT(Sprite,
                                 List<std::pair<u8, u8>>{{0, 0}, {1, 0}, {2, 0}},
                                 200),
            ECS_CREATE_COMPONENT(Hovering),
            ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<BirdController>()),
        });

    ECSCreateEntity(
        "score-board",
        {
            ECS_CREATE_COMPONENT(Geometry, 20, 20),
            ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<ScoreBoard>()),
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
    NTT_APP_CONFIG(LogLevel::DEBUG, LOGGER_CONSOLE);

    auto windowSize = GetWindowSize();

    BeginLayer(UI_LAYER_0);
    ECSCreateEntity(
        "resume-btn",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, windowSize.height / 2, 100),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("menu-btn"), 1, 1,
                                 PRIORITY_0,
                                 "Resume"),
            ECS_CREATE_COMPONENT(Hovering),
            ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<ResumeButtonController>()),
        });

    RegisterEvent(
        NTT_LAYER_CHANGED,
        [](auto id, void *sender, EventContext context)
        {
            auto layer = context.u16_data[0];
            NTT_APP_DEBUG("Layer {} is changed", layer);
        });

    RegisterEvent(
        NTT_DEBUG_CONTINUE,
        [](...)
        {
            NTT_APP_DEBUG("The debugging is continued");
        });

    RegisterEvent(
        NTT_DEBUG_CHOOSE_ENTITY,
        [](auto id, void *sender, EventContext context)
        {
            auto entityId = context.u32_data[0];
            auto geo = ECS_GET_COMPONENT(entityId, Geometry);
            NTT_APP_DEBUG("The entity {} with geo: ({}, {}, {}, {}, {})",
                          entityId,
                          geo->x,
                          geo->y,
                          geo->width,
                          geo->height,
                          geo->rotation);
        });
}

void MainLoop(f32 delta)
{
    if (CheckState(NTT_KEY_D, NTT_PRESS))
    {
        // NTT_APP_DEBUG("The key D is pressed");
        DebugBreak();
    }
}

void Close()
{
}