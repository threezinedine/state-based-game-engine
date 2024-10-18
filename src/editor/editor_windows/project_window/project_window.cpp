#include "project_window.hpp"
#include <NTTEngine/platforms/utils.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

namespace ntt
{
    class ProjectWindow::Impl
    {
    public:
        Ref<ProjectInfo> project;

        i32 width;
        i32 height;
        char title[256];
        char defaultSceneName[256];

        void ReloadProject()
        {
            width = project->width;
            height = project->height;
            memcpy(title, project->title.RawString().c_str(), project->title.Length());
            memcpy(defaultSceneName, project->defaultSceneName.RawString().c_str(),
                   sizeof(defaultSceneName));
        }
    };

    ProjectWindow::ProjectWindow(Ref<ProjectInfo> project)
        : OpenClosableWindow("Project"),
          m_impl(CreateScope<Impl>())
    {
        m_impl->project = project;
    }

    ProjectWindow::~ProjectWindow()
    {
    }

    void ProjectWindow::OnReloadProject()
    {
        m_impl->ReloadProject();
    }

    void ProjectWindow::InitImpl()
    {
        m_impl->ReloadProject();
    }

    void ProjectWindow::UpdateImpl(b8 *p_open, ImGuiWindowFlags flags)
    {
        if (ImGui::Begin("Project", p_open, flags))
        {
            ImGui::InputText("Title", m_impl->title, 256);
            ImGui::InputInt("Width", &m_impl->width);
            ImGui::InputInt("Height", &m_impl->height);

            auto sceneNames = m_impl->project->scenes.Keys();
            if (ImGui::BeginCombo("Default Scene", m_impl->defaultSceneName))
            {
                for (auto &sceneName : sceneNames)
                {
                    b8 isSelected = sceneName == m_impl->defaultSceneName;
                    if (ImGui::Selectable(sceneName.RawString().c_str(), isSelected))
                    {
                        memcpy(m_impl->defaultSceneName, sceneName.RawString().c_str(),
                               sizeof(m_impl->defaultSceneName));
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            if (ImGui::Button("Save"))
            {
                ImGui::OpenPopup("changeProjectInfo");
            }
        }

        if (ImGui::BeginPopupModal("changeProjectInfo"))
        {
            ImGui::Text("Are you sure to change the project info? The editor will be reloaded");

            ImGui::Separator();
            if (ImGui::Button("Cancel"))
            {
                m_impl->ReloadProject();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Ok"))
            {
                m_impl->project->title = m_impl->title;
                m_impl->project->width = m_impl->width;
                m_impl->project->height = m_impl->height;
                m_impl->project->defaultSceneName = m_impl->defaultSceneName;
                TriggerEvent(NTT_EDITOR_SAVE_PROJECT);
                ReloadProgram(__FILE__);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }

    void ProjectWindow::ShutdownImpl()
    {
    }
} // namespace ntt
