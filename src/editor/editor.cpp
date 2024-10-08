#include <NTTEngine/editor/editor.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include "rlImGui.h"
#include "imgui.h"
#include <NTTEngine/application/scene_system/scene_system.hpp>

#include "editor_log_window.hpp"
#include "editor_scene_window.hpp"
#include "editor_viewport_window.hpp"

namespace ntt
{
    using namespace log;
    using namespace event;
    using namespace ecs;

    namespace
    {
        b8 s_useEditor = FALSE;
        b8 s_isRunning = FALSE;

        b8 s_shouldStop = FALSE;
        b8 s_shouldStart = FALSE;

        f32 s_screenWidth = 800;
        f32 s_screenHeight = 600;

        b8 s_openLog = TRUE;
        b8 s_openScene = TRUE;

        List<String> s_sceneNames;
        String s_currentScene;
    }

    void EditorInit(b8 use, u16 width, u16 height, List<String> sceneNames)
    {
        s_useEditor = use;

        if (s_useEditor)
        {
            TriggerEvent(NTT_EDITOR_STOP, {});
        }

        s_screenWidth = static_cast<f32>(width);
        s_screenHeight = static_cast<f32>(height);

        rlImGuiSetup(true);
        s_openLog = TRUE;
        s_openScene = TRUE;

        s_sceneNames = sceneNames;
        s_currentScene = s_sceneNames[0];

        EditorViewportWindowInit(s_screenWidth, s_screenHeight);
        EditorLogWindowInit();
        EditorSceneWindowInit();
    }

    void EditorRun()
    {
        if (!s_useEditor)
        {
            return;
        }

        if (s_isRunning)
        {
            return;
        }

        s_shouldStart = TRUE;
        s_isRunning = TRUE;
    }

    void EditorStop()
    {
        if (!s_useEditor)
        {
            return;
        }

        if (!s_isRunning)
        {
            return;
        }

        s_shouldStop = TRUE;
        s_isRunning = FALSE;
    }

    void EditorUpdate(f32 delta)
    {
        if (!s_useEditor)
        {
            return;
        }

        if (s_shouldStart)
        {
            TriggerEvent(NTT_EDITOR_START, {});
            ECSChangeSystemState("Editor System", FALSE);
        }

        if (s_shouldStop)
        {
            TriggerEvent(NTT_EDITOR_STOP, {});
            ECSChangeSystemState("Editor System", TRUE);
        }

        s_shouldStart = FALSE;
        s_shouldStop = FALSE;

        rlImGuiBegin();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Log", NULL, &s_openLog);
                ImGui::MenuItem("Scene", NULL, &s_openScene);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (ImGui::Begin("Toolbars", nullptr, ImGuiWindowFlags_NoTitleBar))
        {
            if (s_isRunning)
            {
                if (ImGui::Button("Stop"))
                {
                    EditorStop();
                }
            }
            else
            {
                if (ImGui::Button("Run"))
                {
                    EditorRun();
                    ImGui::SetWindowFocus("Viewport");
                }
            }

            ImGui::SameLine();

            if (ImGui::BeginCombo(
                    "Scene",
                    s_currentScene.RawString().c_str()))
            {
                for (i32 i = 0; i < s_sceneNames.size(); i++)
                {
                    const b8 isSelected = (s_currentScene == s_sceneNames[i]);
                    if (ImGui::Selectable(s_sceneNames[i].RawString().c_str(), isSelected))
                    {
                        s_currentScene = s_sceneNames[i];
                        SceneOpen(s_currentScene);
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }
        ImGui::End();

        if (s_openScene)
        {
            EditorSceneWindowUpdate(&s_openScene, s_isRunning);
        }

        EditorViewportWindowUpdate(nullptr, s_isRunning);

        static b8 show = TRUE;
        if (show)
        {
            ImGui::ShowDemoWindow(&show);
        }

        if (s_openLog)
        {
            EditorLogWindowUpdate(&s_openLog, s_isRunning);
        }

        rlImGuiEnd();
    }

    void EditorBeginDraw()
    {
        if (!s_useEditor)
        {
            return;
        }

        EditorViewportWindowStartDraw();
    }

    void EditorEndDraw()
    {
        if (!s_useEditor)
        {
            return;
        }

        EditorViewportWindowEndDraw();
    }

    void EditorShutdown()
    {
        EditorLogWindowShutdown();
        EditorSceneWindowShutdown();
        EditorViewportWindowShutdown();
        NTT_ENGINE_INFO("The editor mode is shutdown");
    }
} // namespace ntt
