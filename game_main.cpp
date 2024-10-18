#include "raylib.h"
#include <NTTEngine/NTTEngine.hpp>
#include "imgui.h"
#include "rlImGui.h"

#include <NTTEngine/renderer/ParentSystem.hpp>
#include <NTTEngine/renderer/RenderSystem.hpp>
#include <NTTEngine/renderer/MouseHoveringSystem.hpp>

#include <NTTEngine/physics/collision.hpp>
#include <NTTEngine/physics/collision_system.hpp>
#include <NTTEngine/physics/MassSystem.hpp>

#include <NTTEngine/editor/editor_system.hpp>

#include <NTTEngine/application/script_system/native_script_system.hpp>
#include <NTTEngine/application/script_system/state_system.hpp>
#include <NTTEngine/application/script_system/state_component.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/core/memory.hpp>

using namespace ntt;

void Update();

static Timer s_timer;
static Scope<ProjectInfo> project;
static Scope<SceneInfo> scene;
static Scope<SceneInfo> menu;
String newScene = "";
String menuSceneName = "";
b8 isMenuOpen = FALSE;

int main(void)
{
    MemoryInit();
    LogInit();
    NTT_ENGINE_CONFIG(
        LogLevel::DEBUG,
        {
            CreateRef<ConsoleHandler>(),
        });

    NTT_APP_CONFIG(
        LogLevel::INFO,
        {
            CreateRef<ConsoleHandler>(),
        });

    ProfilingBegin("Initialization");
    EventInit();

    List<String> fileNames = ListFiles(CurrentDirectory());

    project = CreateScope<ProjectInfo>();

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
    SetWindowTitle(project->title.RawString().c_str());
    SetTargetFPS(60);

    ScriptStoreInit("CreateInstance", "DeleteInstance", "GetBaseType");
    AudioInit();
    RendererInit();
    ResourceInit(FALSE);
    InputInit(FALSE, FALSE);

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
        CreateRef<RenderSystem>(FALSE),
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

    ECSBeginLayer(GAME_LAYER);
    ECSBeginLayer(UI_LAYER);
    ECSBeginLayer(EDITOR_LAYER);
    ECSBeginLayer(GAME_LAYER);

    s_timer.Reset();
    project->ReloadDefaultResourcesInfo();
    ResourceLoad(project->defaultResources);

    RegisterEvent(
        NTT_GAME_CHANGE_SCENE,
        [&](event_code_t code, void *sender, const EventContext &context)
        {
            char sceneName[256];
            memcpy(sceneName, sender, context.u32_data[0]);
            newScene = sceneName;
        });

    RegisterEvent(
        NTT_GAME_OPEN_MENU,
        [&](event_code_t code, void *sender, const EventContext &context)
        {
            char menuName[256];
            memcpy(menuName, sender, context.u32_data[0]);
            menuSceneName = menuName;
        });

    RegisterEvent(
        NTT_GAME_CLOSE_MENU,
        [&](event_code_t code, void *sender, const EventContext &context)
        {
            isMenuOpen = FALSE;
        });

    f32 width = static_cast<f32>(project->width);
    f32 height = static_cast<f32>(project->height);
    AddCamera({width / 2, height / 2}, {width, height});

    ProfilingBegin("Update");
    ChangeScene(project->defaultSceneName);
    while (!WindowShouldClose())
    {
        Update();

        if (newScene != "")
        {
            if (scene == nullptr)
            {
                scene = CreateScope<SceneInfo>();
            }

            if (project->scenes.Contains(newScene))
            {
                if (scene != nullptr)
                {
                    // scene->RemoveAllEntities();
                    ECS_ClearLayer(GAME_LAYER);
                    ResourceUnload(scene->resources);
                }

                String sceneCfgFilePath = JoinPath({project->path,
                                                    "scenes",
                                                    format("{}.json", newScene)});

                JSON sceneCfg = JSON(ReadFile(sceneCfgFilePath));
                scene->FromJSON(sceneCfg);
                scene->ReloadResourceInfo();
                ResourceLoad(scene->resources);
                ClearEventsRange(0xA0, 0xAF);
                scene->ReloadEntities();
            }

            newScene = "";
        }

        if (menuSceneName != "")
        {
            if (menu == nullptr)
            {
                menu = CreateScope<SceneInfo>();
            }

            if (project->scenes.Contains(menuSceneName) && !isMenuOpen)
            {
                String sceneCfgFilePath = JoinPath({project->path,
                                                    "scenes",
                                                    format("{}.json", menuSceneName)});

                JSON sceneCfg = JSON(ReadFile(sceneCfgFilePath));
                menu->FromJSON(sceneCfg);
                menu->ReloadResourceInfo();
                ResourceLoad(menu->resources);
                ECSBeginLayer(UI_LAYER);
                menu->ReloadEntities();
                ECSLayerMakeVisible(UI_LAYER);
                isMenuOpen = TRUE;
            }

            menuSceneName = "";
        }

        if (!isMenuOpen)
        {
            if (menu != nullptr)
            {
                ECS_ClearLayer(UI_LAYER);
                // menu->RemoveAllEntities();
                ResourceUnload(menu->resources);
                ECSBeginLayer(GAME_LAYER);
                ECSLayerMakeVisible(GAME_LAYER);
            }
        }
    }

    if (scene != nullptr)
    {
        scene->RemoveAllEntities();
        ResourceUnload(scene->resources);
        scene.reset();
    }

    if (menu != nullptr)
    {
        menu->RemoveAllEntities();
        ResourceUnload(menu->resources);
        menu.reset();
    }

    ResourceUnload(project->defaultResources);
    project.reset();

    ProfilingBegin("Shutdown");
    // EditorShutdown();

    ECSShutdown();

    AudioShutdown();
    InputShutdown();
    ResourceShutdown();
    RendererShutdown();
    HotReloadInit();
    ScriptStoreShutdown();
    HotReloadShutdown();

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