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

        Ref<ProjectInfo> s_project;
        Ref<EditorConfig> s_config;

        Scope<EditorFileDialog> s_newProjectDialog;
        Scope<EditorFileDialog> s_openProjectDialog;
        Scope<EditorFileDialog> s_saveAsProjectDialog;

        Scope<SettingWindow> s_settingWindow;

        List<Scope<OpenClosableWindow>> s_openClosableWindows;
        Scope<NewProjectWindow> s_newProjectWindow;
        Scope<ViewportWindow> s_viewportWindow;

        void OnProjectLoadded(event_code_t code, void *sender, const EventContext &context)
        {
            s_hasProject = TRUE;
            SetWindowTitle(
                s_project->title.RawString().c_str());

            s_viewportWindow = CreateScope<ViewportWindow>(
                s_project->width,
                s_project->height);

            s_viewportWindow->Init();

            s_config->lastProjectFile = JoinPath({s_project->path, s_project->name});
            TriggerEvent(NTT_EDITOR_SAVE_CONFIG);
        }

        void OnHistoryEmpty(event_code_t code, void *sender, const EventContext &context)
        {
            SetWindowTitle(format("{}", s_project->title).RawString().c_str());
        }

        void OnHistoryNotEmpty(event_code_t code, void *sender, const EventContext &context)
        {
            SetWindowTitle(format("{}*", s_project->title).RawString().c_str());
        }

        void OnEditorSaveConfig(event_code_t code, void *sender, const EventContext &context)
        {
            OpenFile(s_config->GetConfigFilePath());
            Write(s_config->ToJSON().ToString());
            CloseFile();
        }

        void OnOpenProject(b8 override)
        {
            JSON project = JSON(ReadFile(s_project->GetProjectFilePath()));
            s_project->From(project);

            TriggerEvent(NTT_EDITOR_PROJECT_LOADED);
        }

        void OnEditorCreateProject(event_code_t code, void *sender, const EventContext &context)
        {
            s_project->defaultResourceFile = "default_resource.json";

            OpenFile(JoinPath({s_project->path, s_project->defaultResourceFile}));
            Write(JSON("[]").ToString());
            CloseFile();

            CreateFolder(JoinPath({s_project->path,
                                   "assets"}));
            CreateFolder(JoinPath({s_project->path,
                                   "assets", "images"}));
            CreateFolder(JoinPath({s_project->path,
                                   "assets", "audios"}));
            CreateFolder(JoinPath({s_project->path,
                                   "scripts"}));

            OpenFile(s_project->GetProjectFilePath());
            Write(s_project->ToJSON().ToString());
            CloseFile();

            String currentGameFile = JoinPath({CurrentDirectory(),
                                               "Game.exe"});

            String targetGameFile = JoinPath({s_project->path,
                                              "Game.exe"});

            NTTCopyFile(currentGameFile, targetGameFile);

            String currentDllFile = JoinPath({CurrentDirectory(),
                                              "libNTTEngine.dll"});

            String targetDllFile = JoinPath({s_project->path,
                                             "libNTTEngine.dll"});

            NTTCopyFile(currentDllFile, targetDllFile);

            NTT_ENGINE_INFO("Project created");
        }

        void OnNewProjectPathIsSelected(b8 override)
        {
            if (override)
            {
                ClearFolder(s_project->path);
            }
            s_newProjectWindow->Open();
        }

        b8 CheckProjectExistance(const String &fileName)
        {
            auto files = ListFiles(GetFileFolder(fileName), FALSE);
            for (auto &file : files)
            {
                if (CheckFileExtension(file, ".nttproj"))
                {
                    return TRUE;
                }
            }
            return FALSE;
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

        s_configFilePath = JoinPath({CurrentDirectory(), "config.json"});

        s_project = CreateRef<ProjectInfo>();
        s_config = CreateRef<EditorConfig>();

        if (IsExist(s_configFilePath))
        {
            auto config = JSON(ReadFile(s_configFilePath));
            s_config->From(config);
        }

        // ========================================
        // Imgui configuration below
        // ========================================

        ImGuiIO &io = ImGui::GetIO();
        io.FontGlobalScale = s_config->editorFontSize;

        // ========================================
        // Imgui configuration above
        // ========================================

        // ========================================
        // Event registration below
        // ========================================
        RegisterEvent(NTT_EDITOR_SAVE_CONFIG, OnEditorSaveConfig);
        RegisterEvent(NTT_EDITOR_SAVE_PROJECT, OnEditorCreateProject);
        RegisterEvent(NTT_EDITOR_PROJECT_LOADED, OnProjectLoadded);
        RegisterEvent(NTT_HISTORY_EMPTY, OnHistoryEmpty);
        RegisterEvent(NTT_HISTORY_NOT_EMPTY, OnHistoryNotEmpty);
        // ========================================
        // Event registration above
        // ========================================

        if (s_config->lastProjectFile != "")
        {
            s_project->path = GetFileFolder(s_config->lastProjectFile);
            s_project->name = GetFileName(s_config->lastProjectFile);

            OnOpenProject(FALSE);
        }

        // ========================================
        // Window creation below
        // ========================================
        s_openClosableWindows.push_back(CreateScope<LogWindow>());
        s_openClosableWindows.push_back(CreateScope<ResourceWindow>(s_project, s_config));

        s_newProjectWindow = CreateScope<NewProjectWindow>(s_project, s_config);
        s_settingWindow = CreateScope<SettingWindow>(s_project, s_config);

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
                [&](b8 override) {
                }});
        // ========================================
        // Window creation above
        // ========================================

        // ========================================
        // Window initialization below
        // ========================================
        s_newProjectWindow->Init();
        s_settingWindow->Init();

        for (auto &window : s_openClosableWindows)
        {
            window->Init();
            window->Open();
        }
        // ========================================
        // Window initialization below
        // ========================================
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

                if (ImGui::MenuItem("Save", "Ctrl+S", false, s_hasProject))
                {
                    NTT_ENGINE_DEBUG("Save");
                }

                if (ImGui::MenuItem("Save As", "Ctrl+Shift+S", false, s_hasProject))
                {
                    s_saveAsProjectDialog->Open();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Preference", "Ctrl+`"))
                {
                    s_settingWindow->Open();
                }

                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4"))
                {
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                for (auto &window : s_openClosableWindows)
                {
                    window->DrawMenuItem();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGuiIO &io = ImGui::GetIO();
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_N)))
        {
            s_newProjectDialog->Open();
        }
        else if (io.KeyCtrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_O)))
        {
            s_openProjectDialog->Open();
        }
        else if (io.KeyCtrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S)))
        {
            NTT_ENGINE_DEBUG("Save");
        }
        else if (io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S)))
        {
            s_saveAsProjectDialog->Open();
        }

        s_newProjectDialog->Update();
        s_openProjectDialog->Update();
        s_saveAsProjectDialog->Update();

        s_newProjectWindow->Update();
        s_settingWindow->Update();

        if (s_viewportWindow != nullptr)
        {
            s_viewportWindow->Update();
        }

        static b8 show = TRUE;
        if (show)
        {
            ImGui::ShowDemoWindow(&show);
        }

        for (auto &window : s_openClosableWindows)
        {
            window->Update();
        }

        rlImGuiEnd();
    }

    void EditorBeginDraw()
    {
        if (s_viewportWindow != nullptr)
        {
            s_viewportWindow->OnStartDraw();
        }
    }

    void EditorEndDraw()
    {
        if (s_viewportWindow != nullptr)
        {
            s_viewportWindow->OnStopDraw();
        }
    }

    void EditorShutdown()
    {
        for (auto &window : s_openClosableWindows)
        {
            window->Shutdown();
        }
        s_openClosableWindows.clear();

        if (s_viewportWindow != nullptr)
        {
            s_viewportWindow->Shutdown();
            s_viewportWindow.reset();
        }

        s_settingWindow->Shutdown();
        s_settingWindow.reset();

        s_newProjectWindow->Shutdown();
        s_newProjectWindow.reset();

        NTT_ENGINE_INFO("The editor mode is shutdown");
    }
} // namespace ntt
