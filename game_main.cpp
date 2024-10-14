#include "raylib.h"
#include <NTTEngine/NTTEngine.hpp>
#include "imgui.h"
#include "rlImGui.h"

#include <NTTEngine/renderer/ParentSystem.hpp>
#include <NTTEngine/renderer/RenderSystem.hpp>
#include <NTTEngine/renderer/MouseHoveringSystem.hpp>

#include <NTTEngine/physics/Collision.hpp>
#include <NTTEngine/physics/MassSystem.hpp>

#include <NTTEngine/editor/editor_system.hpp>

#include <NTTEngine/application/script_system/native_script_system.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/core/memory.hpp>

using namespace ntt;
using namespace ntt::memory;
using namespace ntt::log;

void Update();

static Timer s_timer;

int main(void)
{
    MemoryInit();
    LogInit();
    ProfilingBegin("Initialization");
    EventInit();

    NTT_ENGINE_CONFIG(
        LogLevel::INFO,
        {
            CreateRef<ConsoleHandler>(),
            CreateRef<EditorLogHandler>(),
        });

    NTT_APP_CONFIG(
        LogLevel::INFO,
        {
            CreateRef<ConsoleHandler>(),
            CreateRef<EditorLogHandler>(),
        });

    List<String> fileNames = ListFiles(CurrentDirectory());

    Scope<ProjectInfo> project = CreateScope<ProjectInfo>();

    for (auto fileName : fileNames)
    {
        if (CheckFileExtension(fileName, ".nttproj"))
        {
            project->From(ReadFile(fileName));
            SetWindowSize(
                {static_cast<position_t>(project->width),
                 static_cast<position_t>(project->height)});
            break;
        }
    }

    SetTraceLogLevel(LOG_NONE);
    InitWindow(project->width, project->height, "NTT Engine");
    SetTargetFPS(60);

    AudioInit();
    RendererInit();
    ResourceInit();
    InputInit(FALSE, FALSE);
    ScriptStoreInit("CreateScript", "DeleteScript", "GetBaseType");

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
        CreateRef<RenderSystem>(),
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
    // EditorInit(CurrentDirectory());

    if (project->scenes.Contains(project->defaultSceneName))
    {
        String sceneCfgFilePath = JoinPath({project->path,
                                            "scenes",
                                            format("{}.json", project->defaultSceneName)});
        JSON sceneCfg = JSON(ReadFile(sceneCfgFilePath));
        Scope<SceneInfo> scene = CreateScope<SceneInfo>();
        project->ReloadDefaultResourcesInfo();
        ResourceLoad(project->defaultResources);

        scene->FromJSON(sceneCfg);
        scene->ReloadResourceInfo();
        ResourceLoad(scene->resources);
        scene->ReloadEntities();
    }

    ProfilingBegin("Update");
    while (!WindowShouldClose())
    {
        Update();
    }

    ProfilingBegin("Shutdown");
    // EditorShutdown();

    ECSShutdown();
    ScriptStoreShutdown();

    AudioShutdown();
    InputShutdown();
    ResourceShutdown();
    RendererShutdown();

    CloseWindow();
    NTT_ENGINE_DEBUG("Editor shutdown.");

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
    EndDrawing();
}