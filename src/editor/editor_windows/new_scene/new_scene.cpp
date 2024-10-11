#include "new_scene.hpp"
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/platforms/path.hpp>
#include "imgui.h"
#include <NTTEngine/application/event_system/event_system.hpp>

namespace ntt
{
    using namespace event;

    class NewSceneWindow::Impl
    {
    public:
        Ref<ProjectInfo> project;
        Ref<EditorConfig> config;

        char sceneName[128];
    };

    NewSceneWindow::NewSceneWindow(Ref<ProjectInfo> project, Ref<EditorConfig> config)
        : OpenClosableWindow("New Scene")
    {
        m_impl = CreateScope<Impl>();

        m_impl->project = project;
        m_impl->config = config;
    }

    NewSceneWindow::~NewSceneWindow() {}

    void NewSceneWindow::InitImpl()
    {
    }

    void NewSceneWindow::UpdateImpl(b8 *p_open)
    {
        // centering the window
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        ImVec2 windowSize = ImVec2(200, 200); // Assuming the window size is 200x200
        ImVec2 windowPos = ImVec2(
            (displaySize.x - windowSize.x) / 2,
            (displaySize.y - windowSize.y) / 2);
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);

        if (ImGui::Begin("New Scene", p_open, 0))
        {
            ImGui::Text("New Scene Window");
            ImGui::InputText("Scene Name", m_impl->sceneName, sizeof(m_impl->sceneName));
            ImGui::Separator();

            if (ImGui::Button("Cancel"))
            {
                Close();
            }

            ImGui::SameLine();

            if (ImGui::Button("Ok"))
            {
                m_impl->project->sceneNames.push_back(m_impl->sceneName);
                TriggerEvent(NTT_EDITOR_CREATE_NEW_SCENE);
                Close();
            }

            ImGui::End();
        }
    }

    void NewSceneWindow::ShutdownImpl()
    {
    }
} // namespace ntt
