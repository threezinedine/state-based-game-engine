#include <NTTEngine/editor/editor.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include "rlImGui.h"
#include "imgui.h"
#include "raylib.h"
#include <NTTEngine/application/scene_system/scene_system.hpp>
#include <array>

#include "editor_windows/editor_windows.hpp"

#include "editor_scene/editor_scene_window.hpp"
#include "editor_viewport/editor_viewport_window.hpp"
#include "entity_window/editor_entity_window.hpp"
#include "editor_tool/editor_tool.hpp"
#include <NTTEngine/platforms/path.hpp>
#include "ImGuiFileDialog.h"
#include "editor_project_dialog/editor_project_dialog.hpp"

#include "types.hpp"

namespace ntt
{
    using namespace log;
    using namespace event;
    using namespace ecs;

    namespace
    {
        b8 s_isRunning = FALSE;
        b8 s_hasProject = FALSE;

        b8 s_openLog = TRUE;
        b8 s_openScene = TRUE;
        b8 s_openEntity = TRUE;
        b8 s_newProjectConfig = FALSE;

        List<String> s_sceneNames;
        String s_currentScene;
        String s_assetPath;
        String s_configFilePath;

        // JSON s_project;
        // JSON s_config;

        Ref<ProjectInfo> s_project;
        Ref<EditorConfig> s_config;

        Scope<EditorFileDialog> s_newProjectDialog;
        Scope<EditorFileDialog> s_openProjectDialog;
        Scope<EditorFileDialog> s_saveAsProjectDialog;

        List<Scope<OpenClosableWindow>> s_openClosableWindows;
        Scope<NewProjectWindow> s_newProjectWindow;

        void OnProjectLoadded(event_code_t code, void *sender, const EventContext &context)
        {
            s_hasProject = TRUE;
            SetWindowTitle(
                s_project->title.RawString().c_str());
        }

        void OnEditorSaveConfig(event_code_t code, void *sender, const EventContext &context)
        {
            JSON config = JSON("{}");
            config.Set("lastProjectPath", s_config->lastProjectPath);
            OpenFile(s_configFilePath);
            Write(config.ToString());
            CloseFile();
        }

        void OnOpenProject()
        {
            String projectFile = JoinPath(
                {s_project->path,
                 s_project->name});

            JSON project = JSON(ReadFile(projectFile));

            s_project->title = project.Get<String>("title");
            s_project->width = project.Get<i32>("width", 800);
            s_project->height = project.Get<i32>("height", 600);

            TriggerEvent(NTT_EDITOR_PROJECT_LOADED);
        }

        void OnEditorSaveProject(event_code_t code, void *sender, const EventContext &context)
        {
            String projectFile = JoinPath(
                {s_project->path,
                 s_project->name});

            JSON project = JSON("{}");
            project.Set("name", s_project->name);
            project.Set("path", s_project->path);
            project.Set("width", s_project->width < 0 ? 800 : s_project->width);
            project.Set("height", s_project->height < 0 ? 600 : s_project->height);
            project.Set("title", s_project->title);

            OpenFile(projectFile);
            Write(project.ToString());
            CloseFile();
        }

        void OnNewProjectPathIsSelected()
        {
            s_newProjectWindow->Open();
        }

        b8 CheckProjectExistance(const String &fileName)
        {
            return IsExist(fileName);
        }
    }

    void EditorInit(const String &assetPath)
    {
        rlImGuiSetup(true);
        s_openLog = TRUE;
        s_openScene = TRUE;
        s_openEntity = TRUE;
        s_newProjectConfig = FALSE;
        s_assetPath = assetPath;
        s_hasProject = FALSE;

        // Imgui configuration below
        ImGuiIO &io = ImGui::GetIO();
        io.FontGlobalScale = 2.0f;
        // Imgui configuration above

        s_configFilePath = JoinPath({CurrentDirectory(), "config.json"});

        s_project = CreateRef<ProjectInfo>();
        s_config = CreateRef<EditorConfig>();

        if (IsExist(s_configFilePath))
        {
            auto config = JSON(ReadFile(s_configFilePath));
            s_config->lastProjectPath = config.Get<String>("lastProjectPath", ".");
        }

        s_openClosableWindows.push_back(CreateScope<LogWindow>());

        s_newProjectWindow = CreateScope<NewProjectWindow>(s_project, s_config);
        // EditorSceneWindowInit();
        // EditorEntityWindowInit();
        // EditorToolInit();

        RegisterEvent(NTT_EDITOR_SAVE_CONFIG, OnEditorSaveConfig);
        RegisterEvent(NTT_EDITOR_SAVE_PROJECT, OnEditorSaveProject);
        RegisterEvent(NTT_EDITOR_PROJECT_LOADED, OnProjectLoadded);

        s_newProjectDialog = CreateScope<EditorFileDialog>(
            s_project,
            s_config,
            EditorFileDialogData{
                "newProject",
                "Create a new project",
                CheckProjectExistance,
                OnNewProjectPathIsSelected});

        s_openProjectDialog = CreateScope<EditorFileDialog>(
            s_project,
            s_config,
            EditorFileDialogData{
                "openProject",
                "Open the project",
                nullptr,
                OnOpenProject});

        s_saveAsProjectDialog = CreateScope<EditorFileDialog>(
            s_project,
            s_config,
            EditorFileDialogData{
                "saveAsProject",
                "Save the project as",
                nullptr,
                [&]() {
                }});

        s_newProjectWindow->Init();

        for (auto &window : s_openClosableWindows)
        {
            window->Init();
            window->Open();
        }
    }

    void EditorUpdate(f32 delta)
    {
        rlImGuiBegin();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                {
                    s_newProjectDialog->Open();
                }

                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                    s_openProjectDialog->Open();
                }

                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    NTT_ENGINE_DEBUG("Save");
                }

                if (ImGui::MenuItem("Save As", "Ctrl+Shift+S", !s_hasProject))
                {
                    s_saveAsProjectDialog->Open();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                // ImGui::MenuItem("Log", NULL, &s_openLog);
                for (auto &window : s_openClosableWindows)
                {
                    window->DrawMenuItem();
                }
                ImGui::MenuItem("Scene", NULL, &s_openScene);
                ImGui::MenuItem("Entity", NULL, &s_openEntity);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        s_newProjectDialog->Update();
        s_openProjectDialog->Update();
        s_saveAsProjectDialog->Update();

        s_newProjectWindow->Update();

        // if (ImGui::Begin("Toolbars", nullptr, ImGuiWindowFlags_NoTitleBar))
        // {
        //     if (s_isRunning)
        //     {
        //         if (ImGui::Button("Stop"))
        //         {
        //             EditorStop();
        //         }
        //     }
        //     else
        //     {
        //         if (ImGui::Button("Run"))
        //         {
        //             EditorRun();
        //             ImGui::SetWindowFocus("Viewport");
        //         }
        //     }

        //     ImGui::SameLine();

        //     if (ImGui::BeginCombo(
        //             "Scene",
        //             s_currentScene.RawString().c_str(),
        //             ImGuiComboFlags_WidthFitPreview))
        //     {
        //         for (i32 i = 0; i < s_sceneNames.size(); i++)
        //         {
        //             const b8 isSelected = (s_currentScene == s_sceneNames[i]);
        //             if (ImGui::Selectable(s_sceneNames[i].RawString().c_str(), isSelected))
        //             {
        //                 s_currentScene = s_sceneNames[i];
        //                 SceneOpen(s_currentScene);
        //             }

        //             if (isSelected)
        //             {
        //                 ImGui::SetItemDefaultFocus();
        //             }
        //         }
        //         ImGui::EndCombo();
        //     }

        //     ImGui::SameLine();

        //     static std::array<const char *, 10> layers = {
        //         "Game",
        //         "UI_0",
        //         "UI_1",
        //         "UI_2",
        //         "UI_3",
        //         "UI_4",
        //         "UI_5",
        //         "UI_6",
        //         "UI_7",
        //         "UI_8",
        //     };
        //     static u32 currentLayer = GAME_LAYER;

        //     if (ImGui::BeginCombo("Layers", layers[currentLayer], ImGuiComboFlags_WidthFitPreview))
        //     {
        //         for (i32 i = 0; i < layers.size(); i++)
        //         {
        //             const b8 isSelected = (i == currentLayer);
        //             if (ImGui::Selectable(layers[i], isSelected))
        //             {
        //                 ECSLayerMakeVisible(static_cast<layer_t>(i));
        //                 ECSLayerMakeVisible(static_cast<layer_t>(EDITOR_LAYER));
        //                 currentLayer = i;
        //             }

        //             if (isSelected)
        //             {
        //                 ImGui::SetItemDefaultFocus();
        //             }
        //         }

        //         ImGui::EndCombo();
        //     }
        // }
        // ImGui::End();

        // if (s_openScene)
        // {
        //     EditorSceneWindowUpdate(&s_openScene, s_isRunning);
        // }

        // EditorViewportWindowUpdate(nullptr, s_isRunning);

        static b8 show = TRUE;
        if (show)
        {
            ImGui::ShowDemoWindow(&show);
        }

        // EditorToolUpdate();

        for (auto &window : s_openClosableWindows)
        {
            window->Update();
        }

        // if (s_openEntity)
        // {
        //     EditorEntityWindowUpdate(&s_openEntity, s_isRunning);
        // }

        rlImGuiEnd();
    }

    void EditorBeginDraw()
    {
        // EditorViewportWindowStartDraw();
    }

    void EditorEndDraw()
    {
        // EditorViewportWindowEndDraw();
    }

    void EditorShutdown()
    {
        for (auto &window : s_openClosableWindows)
        {
            window->Shutdown();
            window.reset();
        }

        s_newProjectWindow->Shutdown();
        s_newProjectWindow.reset();

        // EditorEntityWindowShutdown();
        // EditorSceneWindowShutdown();
        // EditorViewportWindowShutdown();
        // EditorToolShutdown();
        NTT_ENGINE_INFO("The editor mode is shutdown");
    }
} // namespace ntt
