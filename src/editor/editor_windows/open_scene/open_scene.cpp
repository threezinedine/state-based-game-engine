#include "open_scene.hpp"
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/platforms/path.hpp>
#include "imgui.h"
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    class OpenSceneWindow::Impl
    {
    public:
        Ref<ProjectInfo> project;
        Ref<EditorConfig> config;
        Ref<SceneInfo> scene;
        List<String> sceneNames;
        u32 index;
    };

    OpenSceneWindow::OpenSceneWindow(Ref<ProjectInfo> project,
                                     Ref<EditorConfig> config,
                                     Ref<SceneInfo> scene)
        : OpenClosableWindow("New Scene")
    {
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();

        m_impl->project = project;
        m_impl->config = config;
        m_impl->scene = scene;
        m_impl->sceneNames = project->scenes.Keys();
        m_impl->index = 0;
    }

    OpenSceneWindow::~OpenSceneWindow()
    {
        PROFILE_FUNCTION();
    }

    void OpenSceneWindow::InitImpl()
    {
        PROFILE_FUNCTION();
    }

    void OpenSceneWindow::UpdateImpl(b8 *p_open, ImGuiWindowFlags flags)
    {
        PROFILE_FUNCTION();
        if (ImGui::Begin("Open Scene", p_open))
        {
            if (m_impl->sceneNames.size() == 0)
            {
                ImGui::Text("Project has not scene to choose");
            }
            else
            {
                if (ImGui::BeginCombo(
                        "Select scene",
                        m_impl->sceneNames[m_impl->index].RawString().c_str()))
                {
                    for (u32 i = 0; i < m_impl->sceneNames.size(); i++)
                    {
                        b8 isSelected = m_impl->index == i;
                        if (ImGui::Selectable(
                                m_impl->sceneNames[i].RawString().c_str(),
                                isSelected))
                        {
                            m_impl->index = i;
                        }

                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }

                    ImGui::EndCombo();
                }

                ImGui::Separator();
                if (ImGui::Button("Cancel"))
                {
                    Close();
                }
                ImGui::SameLine();
                if (ImGui::Button("Ok"))
                {
                    m_impl->scene->sceneName = m_impl->sceneNames[m_impl->index];
                    TriggerEvent(NTT_EDITOR_OPEN_SCENE);
                    Close();
                }
            }
        }
        ImGui::End();
    }

    void OpenSceneWindow::OnReloadProject()
    {
        PROFILE_FUNCTION();
        m_impl->sceneNames = m_impl->project->scenes.Keys();
    }

    void OpenSceneWindow::OnReloadScene()
    {
        PROFILE_FUNCTION();
        m_impl->sceneNames = m_impl->project->scenes.Keys();
    }

    void OpenSceneWindow::ShutdownImpl()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt
