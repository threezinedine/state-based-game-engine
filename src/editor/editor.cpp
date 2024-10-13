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
        Ref<SceneInfo> s_scene;

        Ref<EditorFileDialog> s_newProjectDialog;
        Ref<EditorFileDialog> s_openProjectDialog;
        Ref<EditorFileDialog> s_saveAsProjectDialog;

        Ref<SettingWindow> s_settingWindow;
        Ref<NewProjectWindow> s_newProjectWindow;
        Ref<ViewportWindow> s_viewportWindow;
        Ref<LogWindow> s_logWindow;
        Ref<ResourceWindow> s_resourceWindow;
        Ref<NewSceneWindow> s_newSceneWindow;
        Ref<SceneWindow> s_sceneWindow;
        Ref<OpenSceneWindow> s_openSceneWindow;

        List<Ref<EditorWindow>> s_normalWindows;
        List<Ref<ProjectReloadWindow>> s_reloadWindows;
        List<Ref<SceneReloadWindow>> s_sceneChangeWindows;
        List<Ref<OpenClosableWindow>> s_openClosableWindows;

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

            for (auto &window : s_reloadWindows)
            {
                window->OnReloadProject();
            }
        }

        void OnSceneChanged(event_code_t code, void *sender, const EventContext &context)
        {
            if (s_project->scenes.Contains(s_scene->sceneName))
            {
                s_scene->filePath = s_project->scenes[s_scene->sceneName]->filePath;
            }

            for (auto &window : s_sceneChangeWindows)
            {
                window->OnReloadScene();
            }
        }

        void OnNewSceneCreated(event_code_t code, void *sender, const EventContext &context)
        {
            TriggerEvent(NTT_EDITOR_OPEN_SCENE);
            TriggerEvent(NTT_EDITOR_SAVE_PROJECT);
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

        void OnSaveProject(event_code_t code, void *sender, const EventContext &context)
        {
            OpenFile(s_project->GetProjectFilePath());
            Write(s_project->ToJSON().ToString());
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
            TriggerEvent(NTT_EDITOR_SAVE_PROJECT);
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
        s_scene = CreateRef<SceneInfo>();

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
        RegisterEvent(NTT_EDITOR_CREATE_PROJECT, OnEditorCreateProject);
        RegisterEvent(NTT_EDITOR_PROJECT_LOADED, OnProjectLoadded);
        RegisterEvent(NTT_HISTORY_EMPTY, OnHistoryEmpty);
        RegisterEvent(NTT_HISTORY_NOT_EMPTY, OnHistoryNotEmpty);
        RegisterEvent(NTT_EDITOR_CREATE_NEW_SCENE, OnNewSceneCreated);
        RegisterEvent(NTT_EDITOR_SAVE_PROJECT, OnSaveProject);
        RegisterEvent(NTT_EDITOR_OPEN_SCENE, OnSceneChanged);
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

        s_logWindow = CreateRef<LogWindow>();
        s_openClosableWindows.push_back(s_logWindow);
        s_normalWindows.push_back(s_logWindow);
        s_reloadWindows.push_back(s_logWindow);

        s_resourceWindow = CreateRef<ResourceWindow>(s_project, s_config, s_scene);
        s_openClosableWindows.push_back(s_resourceWindow);
        s_normalWindows.push_back(s_resourceWindow);
        s_reloadWindows.push_back(s_resourceWindow);
        s_sceneChangeWindows.push_back(s_resourceWindow);

        s_newProjectWindow = CreateRef<NewProjectWindow>(s_project, s_config);
        s_normalWindows.push_back(s_newProjectWindow);

        s_settingWindow = CreateRef<SettingWindow>(s_project, s_config);
        s_normalWindows.push_back(s_settingWindow);

        s_newSceneWindow = CreateRef<NewSceneWindow>(s_project, s_config, s_scene);
        s_normalWindows.push_back(s_newSceneWindow);

        s_sceneWindow = CreateRef<SceneWindow>(s_project, s_scene, s_config);
        s_normalWindows.push_back(s_sceneWindow);
        s_openClosableWindows.push_back(s_sceneWindow);
        s_reloadWindows.push_back(s_sceneWindow);
        s_sceneChangeWindows.push_back(s_sceneWindow);

        s_openSceneWindow = CreateRef<OpenSceneWindow>(s_project, s_config, s_scene);
        s_normalWindows.push_back(s_openSceneWindow);
        s_reloadWindows.push_back(s_openSceneWindow);
        s_sceneChangeWindows.push_back(s_openSceneWindow);

        s_normalWindows.push_back(s_viewportWindow);

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
        for (auto &window : s_normalWindows)
        {
            if (window != nullptr)
            {
                window->Init();
            }
        }

        for (auto &window : s_openClosableWindows)
        {
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
                if (ImGui::BeginMenu("New"))
                {
                    if (ImGui::MenuItem("New Project", "Ctrl+Shift+N"))
                    {
                        s_newProjectDialog->Open();
                    }

                    if (ImGui::MenuItem("New Scene", "Ctrl+N", false, s_hasProject))
                    {
                        s_newSceneWindow->Open();
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Open"))
                {
                    if (ImGui::MenuItem("Open", "Ctrl+Shift+O"))
                    {
                        s_openProjectDialog->Open();
                    }

                    if (ImGui::MenuItem("Open Scene", "Ctrl+O", false, s_hasProject))
                    {
                        s_openSceneWindow->Open();
                    }

                    ImGui::EndMenu();
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
        if (io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_N)))
        {
            s_newProjectDialog->Open();
        }
        else if (io.KeyCtrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_N)))
        {
            s_newSceneWindow->Open();
        }
        else if (io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_O)))
        {
            s_openProjectDialog->Open();
        }
        else if (io.KeyCtrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_O)))
        {
            s_openSceneWindow->Open();
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

        static b8 show = TRUE;
        if (show)
        {
            ImGui::ShowDemoWindow(&show);
        }

        for (auto &window : s_normalWindows)
        {
            if (window != nullptr)
            {
                window->Update();
            }
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
        for (auto &window : s_normalWindows)
        {
            if (window != nullptr)
            {
                window->Shutdown();
            }
        }
        s_openClosableWindows.clear();

        s_normalWindows.clear();
        s_reloadWindows.clear();
        s_openClosableWindows.clear();

        s_project.reset();
        s_config.reset();

        s_scene->entities.clear();
        s_scene.reset();

        NTT_ENGINE_INFO("The editor mode is shutdown");
    }
} // namespace ntt
