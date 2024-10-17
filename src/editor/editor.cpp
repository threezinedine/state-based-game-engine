#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/editor/editor.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include "rlImGui.h"
#include "imgui.h"
#include "raylib.h"
#include <NTTEngine/application/scene_system/scene_system.hpp>
#include <NTTEngine/platforms/application.hpp>
#include <array>

#include <NTTEngine/editor/EditorWindow.hpp>
#include <NTTEngine/editor/OpenClosableWindow.hpp>
#include <NTTEngine/editor/ProjectReloadWindow.hpp>
#include <NTTEngine/editor/SceneReloadWindow.hpp>
#include "editor_windows/editor_windows.hpp"
#include <NTTEngine/editor/editor_clipboard.hpp>
#include <NTTEngine/application/hot_reload_module/hot_reload_module.hpp>

#include <NTTEngine/platforms/path.hpp>
#include "ImGuiFileDialog.h"
#include "editor_project_dialog/editor_project_dialog.hpp"
#include <thread>

#include <NTTEngine/editor/types.hpp>

namespace ntt
{
    namespace
    {
        b8 s_isRunning = FALSE;
        b8 s_hasProject = FALSE;
        b8 s_hanging = FALSE;
        List<String> s_changedFiles = {};

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
        Ref<EntityWindow> s_entityWindow;

        List<Ref<EditorWindow>> s_normalWindows;
        List<Ref<ProjectReloadWindow>> s_reloadWindows;
        List<Ref<SceneReloadWindow>> s_sceneChangeWindows;
        List<Ref<OpenClosableWindow>> s_openClosableWindows;

        void OnProjectLoadded(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            s_hasProject = TRUE;
            SetWindowTitle(
                s_project->title.RawString().c_str());
            SetWindowSize({static_cast<ntt_size_t>(s_project->width),
                           static_cast<ntt_size_t>(s_project->height)});

            HotReload_SetProjectPath(s_project->path);
            Resource_SetProjectPath(s_project->path);

            s_viewportWindow = CreateScope<ViewportWindow>(
                s_project->width,
                s_project->height);

            s_viewportWindow->Init();

            s_config->lastProjectFile = JoinPath({s_project->path, s_project->name});
            TriggerEvent(NTT_EDITOR_SAVE_CONFIG);
            TriggerEvent(NTT_SCENE_CHANGED);

            for (auto &window : s_reloadWindows)
            {
                window->OnReloadProject();
            }
        }

        void OnSceneChanged(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();

            char sceneName[256];
            String newSceneName;
            if (sender != nullptr)
            {
                memcpy(sceneName, sender, context.u32_data[0]);
                newSceneName = sceneName;
            }

            ResourceUnload(s_project->defaultResources);
            s_project->ReloadDefaultResourcesInfo();
            ResourceLoad(s_project->defaultResources);

            if (s_project->scenes.Contains(s_scene->sceneName))
            {
                s_scene->RemoveAllEntities();
                ResourceUnload(s_scene->resources);
            }

            if (s_project->scenes.Contains(newSceneName) && newSceneName != "")
            {
                s_scene->filePath = s_project->scenes[newSceneName]->filePath;
                s_scene->sceneName = newSceneName;
            }

            if (s_project->scenes.Contains(s_scene->sceneName))
            {
                s_scene->ReloadResourceInfo();
                ResourceLoad(s_scene->resources);
                s_scene->ReloadEntities();
            }

            for (auto &window : s_sceneChangeWindows)
            {
                window->OnReloadScene();
            }
        }

        void OnNewSceneCreated(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            TriggerEvent(NTT_EDITOR_OPEN_SCENE);
            TriggerEvent(NTT_EDITOR_SAVE_PROJECT);
        }

        void OnSaveScene(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            if (s_scene->sceneName == "")
            {
                return;
            }

            s_scene->SaveEntitiesInfo();
        }

        void OnHistoryEmpty(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            SetWindowTitle(format("{}", s_project->title).RawString().c_str());
        }

        void OnHistoryNotEmpty(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            SetWindowTitle(format("{}*", s_project->title).RawString().c_str());
        }

        void OnEditorSaveConfig(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            OpenFile(s_config->GetConfigFilePath());
            Write(s_config->ToJSON().ToString());
            CloseFile();
        }

        void OnEditorStartGame(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            if (s_scene->sceneName == "")
            {
                return;
            }

            if (s_isRunning)
            {
                return;
            }

            s_isRunning = TRUE;
            ECSLayerMakeVisible(GAME_LAYER);
            ImGui::SetWindowFocus("Viewport");
        }

        void OnEditorStopGame(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            if (s_scene->sceneName == "")
            {
                return;
            }

            if (!s_isRunning)
            {
                return;
            }

            s_isRunning = FALSE;
            ECSLayerMakeVisible(EDITOR_LAYER);
            s_scene->RemoveAllEntities();
            s_scene->ReloadEntities();
        }

        void OnSaveProject(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            OpenFile(s_project->GetProjectFilePath());
            Write(s_project->ToJSON().ToString());
            CloseFile();
        }

        void OnOpenProject(b8 override)
        {
            PROFILE_FUNCTION();
            JSON project = JSON(ReadFile(s_project->GetProjectFilePath()));
            s_project->From(project);

            TriggerEvent(NTT_EDITOR_PROJECT_LOADED);
        }

        void OnEditorCreateProject(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            s_project->defaultResourceFile = "default_resource.json";
            s_project->defaultSceneName = "";
            s_project->scenes.clear();

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
            PROFILE_FUNCTION();
            if (override)
            {
                ClearFolder(s_project->path);
            }
            s_newProjectWindow->Open();
        }

        b8 CheckProjectExistance(const String &fileName)
        {
            PROFILE_FUNCTION();
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

        void CompileFile(String cppFile)
        {
            if (s_scene->sceneName != "")
            {
                s_scene->RemoveAllEntities();
                ResourceUnload(s_scene->resources);
            }

            ResourceUnload(s_project->defaultResources);

            String outputFile = GetFileName(cppFile);
            outputFile.Replace(".cpp", ".dll");
            outputFile = JoinPath({s_project->path, outputFile});

            auto command = format(
                "g++ -g -o \"{}\" -I \"{}\" -L\"{}\" \"{}\" -lNTTEngine -shared",
                outputFile,
                JoinPath({CurrentDirectory(), "include"}),
                CurrentDirectory(),
                cppFile);

            try
            {
                std::system(command.RawString().c_str());
            }
            catch (const std::exception &e)
            {
                NTT_ENGINE_FATAL(
                    "The file {} cannot be compiled with error {}",
                    cppFile,
                    e.what());
            }

            ResourceLoad(s_project->defaultResources);

            // if (s_scene->sceneName != "")
            // {
            //     TriggerEvent(NTT_EDITOR_OPEN_SCENE);
            // }
        }

        void OnWatchFileChanged(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            char fileName[256];
            u32 length = context.u32_data[0];

            memcpy(fileName, sender, length);
            String file = String(fileName);

            s_changedFiles.push_back(file);

            std::thread t1(
                [](String fileName)
                {
                    EventContext context;
                    context.u32_data[0] = fileName.Length() + 1;
                    CompileFile(fileName);
                    TriggerEvent(NTT_WATCHED_FILE_HANDLED, fileName.RawString().data(), context); },
                file);

            t1.detach();
        }

        void OnWatchingFileHandleComplete(event_code_t code, void *sender, const EventContext &context)
        {
            NTT_ENGINE_DEBUG("The file has been handled");

            char fileName[256];
            u32 length = context.u32_data[0];

            memcpy(fileName, sender, length);
            String file = String(fileName);

            s_changedFiles.RemoveItem(file);
        }
    }

    void EditorInit(const String &assetPath)
    {
        PROFILE_FUNCTION();
        rlImGuiSetup(true);
        s_openLog = TRUE;
        s_openScene = TRUE;
        s_openEntity = TRUE;
        s_newProjectConfig = FALSE;
        s_assetPath = assetPath;
        s_hasProject = FALSE;
        s_hanging = FALSE;

        s_changedFiles.clear();

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
        // Needed system initialization below
        // ========================================
        EditorClipboard_Init();
        // ========================================
        // Needed system initialization above
        // ========================================

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
        RegisterEvent(NTT_EDITOR_START_GAME, OnEditorStartGame);
        RegisterEvent(NTT_EDITOR_STOP_GAME, OnEditorStopGame);
        RegisterEvent(NTT_EDITOR_SAVE_CONFIG, OnEditorSaveConfig);
        RegisterEvent(NTT_EDITOR_CREATE_PROJECT, OnEditorCreateProject);
        RegisterEvent(NTT_EDITOR_PROJECT_LOADED, OnProjectLoadded);
        RegisterEvent(NTT_HISTORY_EMPTY, OnHistoryEmpty);
        RegisterEvent(NTT_HISTORY_NOT_EMPTY, OnHistoryNotEmpty);
        RegisterEvent(NTT_EDITOR_CREATE_NEW_SCENE, OnNewSceneCreated);
        RegisterEvent(NTT_EDITOR_SAVE_PROJECT, OnSaveProject);
        RegisterEvent(NTT_EDITOR_OPEN_SCENE, OnSceneChanged);
        RegisterEvent(NTT_EDITOR_SAVE_SCENE, OnSaveScene);
        RegisterEvent(NTT_WATCHED_FILE_CHANGED, OnWatchFileChanged);
        RegisterEvent(NTT_WATCHED_FILE_HANDLED, OnWatchingFileHandleComplete);
        RegisterEvent(
            NTT_GAME_CHANGE_SCENE,
            [&](event_code_t code, void *sender, const EventContext &context)
            {
                TriggerEvent(NTT_EDITOR_STOP_GAME);
            });
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

        s_entityWindow = CreateRef<EntityWindow>(s_project, s_scene, s_config);
        s_normalWindows.push_back(s_entityWindow);
        s_openClosableWindows.push_back(s_entityWindow);
        s_reloadWindows.push_back(s_entityWindow);
        s_sceneChangeWindows.push_back(s_entityWindow);

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
        PROFILE_FUNCTION();
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

        ImGui::Begin("Scene Run", NULL, ImGuiWindowFlags_NoTitleBar);
        if (s_isRunning)
        {
            if (ImGui::Button("Stop"))
            {
                TriggerEvent(NTT_EDITOR_STOP_GAME);
            }
        }
        else
        {
            if (ImGui::Button("Run"))
            {
                TriggerEvent(NTT_EDITOR_START_GAME);
            }
        }

        List<String> sceneNames = s_project->scenes.Keys();

        ImGui::SameLine();
        ImGui::BeginDisabled(s_isRunning);
        if (ImGui::BeginCombo("scene",
                              s_scene->sceneName == ""
                                  ? "No scene selected"
                                  : s_scene->sceneName.RawString().c_str(),
                              ImGuiComboFlags_WidthFitPreview))
        {
            for (auto &sceneName : sceneNames)
            {
                b8 isSelected = s_scene->sceneName == sceneName;
                if (ImGui::Selectable(sceneName.RawString().c_str(), isSelected))
                {
                    EventContext context;
                    context.u32_data[0] = sceneName.Length() + 1;
                    TriggerEvent(NTT_EDITOR_OPEN_SCENE, sceneName.RawString().data(), context);
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();
        ImGui::End();

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
                window->Update(s_isRunning ? ImGuiWindowFlags_NoInputs : 0);
            }
        }

        if (s_changedFiles.size() != 0 && !s_hanging)
        {
            s_hanging = TRUE;
            ImGui::OpenPopup("hanging");
        }

        if (ImGui::BeginPopupModal("hanging"))
        {
            ImGui::Text("Some thing happens, wait....");

            ImGui::BeginDisabled(s_changedFiles.size() != 0);
            if (ImGui::Button("Ok"))
            {
                s_hanging = FALSE;
                String newScene = s_scene->sceneName;
                EventContext context;
                context.u32_data[0] = newScene.Length() + 1;
                TriggerEvent(NTT_EDITOR_OPEN_SCENE, newScene.RawString().data(), context);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }

        rlImGuiEnd();
    }

    void EditorBeginDraw()
    {
        PROFILE_FUNCTION();
        if (s_viewportWindow != nullptr)
        {
            s_viewportWindow->OnStartDraw();
        }
    }

    void EditorEndDraw()
    {
        PROFILE_FUNCTION();
        if (s_viewportWindow != nullptr)
        {
            s_viewportWindow->OnStopDraw();
        }
    }

    void EditorShutdown()
    {
        PROFILE_FUNCTION();
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

        s_newProjectDialog.reset();
        s_openProjectDialog.reset();
        s_saveAsProjectDialog.reset();

        EditorClipboard_Shutdown();

        NTT_ENGINE_INFO("The editor mode is shutdown");
    }
} // namespace ntt
