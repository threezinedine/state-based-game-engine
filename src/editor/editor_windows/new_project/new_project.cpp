#include "new_project.hpp"
#include "imgui.h"
#include <NTTEngine/application/event_system/event_system.hpp>

namespace ntt
{
    using namespace event;

    class NewProjectWindow::Impl
    {
    public:
        Ref<ProjectInfo> project;
        Ref<EditorConfig> config;

        b8 isRunning = FALSE;

        char title[128] = "NTT Game";
    }; // namespace

    NewProjectWindow::NewProjectWindow(Ref<ProjectInfo> project, Ref<EditorConfig> config)
        : m_impl(CreateScope<Impl>())
    {
        m_impl->project = project;
        m_impl->config = config;
    }

    NewProjectWindow::~NewProjectWindow()
    {
    }

    void NewProjectWindow::InitImpl()
    {
        m_impl->isRunning = FALSE;
    }

    void NewProjectWindow::UpdateImpl(b8 *p_open)
    {
        if (ImGui::Begin("New Project Info", p_open, 0))
        {
            ImGui::Text("Create a new project");
            ImGui::Separator();

            ImGui::Text(format("Project path: {}", m_impl->project->path).RawString().c_str());
            ImGui::Separator();
            ImGui::InputText("Title", m_impl->title, sizeof(m_impl->title));
            ImGui::InputInt("Width", &m_impl->project->width);
            ImGui::InputInt("Height", &m_impl->project->height);

            if (ImGui::Button("Create"))
            {
                m_impl->project->title = m_impl->title;
                TriggerEvent(NTT_EDITOR_PROJECT_LOADED);
                TriggerEvent(NTT_EDITOR_SAVE_PROJECT);
                Close();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
            {
                Close();
            }
        }
        ImGui::End();
    }

    void NewProjectWindow::ShutdownImpl()
    {
    }

} // namespace ntt