#include "setting.hpp"
#include "imgui.h"
#include <NTTEngine/application/event_system/event_system.hpp>

namespace ntt
{
    using namespace event;

    class SettingWindow::Impl
    {
    public:
        Ref<ProjectInfo> project;
        Ref<EditorConfig> config;

        EditorConfig tempConfig;
    };

    SettingWindow::SettingWindow(Ref<ProjectInfo> project, Ref<EditorConfig> config)
    {
        m_impl = CreateScope<Impl>();
        m_impl->project = project;
        m_impl->config = config;
        m_impl->tempConfig.editorFontSize = config->editorFontSize;
    }

    SettingWindow::~SettingWindow() {}

    void SettingWindow::InitImpl()
    {
    }

    void SettingWindow::UpdateImpl(b8 *p_open)
    {
        if (ImGui::Begin("Setting", p_open))
        {
            ImGui::Text("Setting Window");
            ImGui::Separator();
            ImGui::InputFloat("Editor Font Size",
                              &m_impl->tempConfig.editorFontSize, 0.1f, 3.0f, "%.1f");

            ImGui::Separator();

            if (ImGui::Button("Cancel"))
            {
                m_impl->tempConfig.editorFontSize = m_impl->config->editorFontSize;
                Close();
            }

            ImGui::SameLine();

            if (ImGui::Button("Apply"))
            {
                m_impl->config->editorFontSize = m_impl->tempConfig.editorFontSize;

                ImGuiIO &io = ImGui::GetIO();
                io.FontGlobalScale = m_impl->config->editorFontSize;

                TriggerEvent(NTT_EDITOR_SAVE_CONFIG);
                Close();
            }
        }

        ImGui::End();
    }

    void SettingWindow::ShutdownImpl()
    {
    }
} // namespace ntt
