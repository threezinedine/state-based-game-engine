#include "raylib.h"
#include <NTTEngine/NTTEngine.hpp>
#include "imgui.h"
#include "rlImGui.h"

#include <NTTEngine/renderer/ParentSystem.hpp>
#include <NTTEngine/renderer/RenderSystem.hpp>
#include <NTTEngine/renderer/MouseHoveringSystem.hpp>
#include <NTTEngine/renderer/Camera.hpp>

#include <NTTEngine/physics/collision.hpp>
#include <NTTEngine/physics/collision_system.hpp>
#include <NTTEngine/physics/MassSystem.hpp>

#include <NTTEngine/editor/editor_system.hpp>

#include <NTTEngine/application/script_system/native_script_system.hpp>
#include <NTTEngine/application/script_system/state_system.hpp>
#include <NTTEngine/application/script_system/state_component.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>

using namespace ntt;

void Update();

static Timer s_timer;

int main(void)
{
    MemoryInit();
    LogInit();
    ProfilingBegin("Initialization");
    EventInit();
    HistoryManager_Init();

    NTT_ENGINE_CONFIG(
        LogLevel::DEBUG,
        {
            CreateRef<ConsoleHandler>(),
            CreateRef<EditorLogHandler>(),
        });

    NTT_APP_CONFIG(
        LogLevel::DEBUG,
        {
            CreateRef<ConsoleHandler>(),
            CreateRef<EditorLogHandler>(),
        });

    SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "NTT Engine");
    SetTargetFPS(60);
    MaximizeWindow();
    HotReloadInit();
    ScriptStoreInit("CreateInstance", "DeleteInstance", "GetBaseType");

    AudioInit();
    RendererInit();
    ResourceInit(TRUE);
    InputInit(FALSE, TRUE);

    ECSInit();

    // Parent system must be front of the render system for
    //      child entity's position to be updated before rendering
    ECSRegister(
        "Parent System",
        CreateRef<ParentSystem>(),
        {typeid(Parent)},
        TRUE);

    ECSRegister(
        "Render System",
        CreateRef<RenderSystem>(TRUE),
        {typeid(Geometry)},
        TRUE);

    ECSRegister(
        "Editor System",
        CreateRef<EditorSystem>(),
        {typeid(Geometry)},
        TRUE);

    ECSRegister(
        "Native Script System",
        CreateRef<ScriptSystem>(),
        {typeid(NativeScriptComponent)});

    ECSRegister(
        "State System",
        CreateRef<StateSystem>(),
        {typeid(StateComponent)});

    ECSRegister(
        COLLISION_NAME,
        CreateRef<CollisionSystem>(),
        {typeid(Geometry), typeid(Collision)});

    ECSRegister(
        "Hovering System",
        CreateRef<MouseHoveringSystem>(),
        {typeid(Hovering), typeid(Geometry)});

    ECSRegister(
        "Mass System",
        CreateRef<MassSystem>(),
        {typeid(Mass), typeid(Geometry)});

    ECSRegister(
        "Sprite Render System",
        CreateRef<SpriteRenderSystem>(),
        {typeid(Sprite), typeid(TextureComponent)});

    s_timer.Reset();
    EditorInit(CurrentDirectory());

    ECSBeginLayer(GAME_LAYER);
    ECSBeginLayer(UI_LAYER);
    ECSBeginLayer(EDITOR_LAYER);
    ECSLayerMakeVisible(EDITOR_LAYER);
    ECSBeginLayer(GAME_LAYER);

    ProfilingBegin("Update");
    while (!WindowShouldClose())
    {
        Update();
    }

    ProfilingBegin("Shutdown");
    EditorShutdown();

    ECSShutdown();

    AudioShutdown();
    InputShutdown();
    ResourceShutdown();
    RendererShutdown();

    ScriptStoreShutdown();
    HotReloadShutdown();
    CloseWindow();
    NTT_ENGINE_DEBUG("Editor shutdown.");

    HistoryManager_Shutdown();
    EventShutdown();
    ProfilingShutdown();
    LogShutdown();
    MemoryShutdown();

    return 0;
}

void Update()
{
    auto delta = static_cast<f32>(s_timer.GetMilliseconds());
    s_timer.Reset();

    BeginDrawing();

    EditorBeginDraw();
    // ===================================================
    // code of the game loop below
    // ===================================================

    ClearBackground(::BLACK);
    InputUpdate(delta);
    MouseHoveringSystemUpdate(delta);

    ECSUpdate(delta);
    GraphicUpdate();

    // ===================================================
    // code of the game loop above
    // ===================================================
    EditorEndDraw();

    EditorUpdate(delta);

    EndDrawing();
}