#include "editor_project_dialog.hpp"
#include "ImGuiFileDialog.h"
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    using namespace event;

    class EditorFileDialog::Impl
    {
    public:
        Ref<ProjectInfo> project;
        Ref<EditorConfig> config;
        EditorFileDialogData data;
        String tempFilePath;
    };

    EditorFileDialog::EditorFileDialog(
        Ref<ProjectInfo> project,
        Ref<EditorConfig> config,
        const EditorFileDialogData &data)
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
        m_impl->project = project;
        m_impl->config = config;
        m_impl->data = data;
    }

    EditorFileDialog::~EditorFileDialog()
    {
        PROFILE_FUNCTION();
    }

    void EditorFileDialog::Open()
    {
        PROFILE_FUNCTION();
        IGFD::FileDialogConfig config;
        config.path = m_impl->config->lastProjectPath.RawString();

        ImGuiFileDialog::Instance()->OpenDialog(
            m_impl->data.id.RawString().c_str(),
            m_impl->data.windowTitle.RawString().c_str(),
            ".nttproj",
            config);
    }

    void EditorFileDialog::Update()
    {
        PROFILE_FUNCTION();
        if (ImGuiFileDialog::Instance()->Display(m_impl->data.id.RawString().c_str()))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                String fileName = ImGuiFileDialog::Instance()->GetFilePathName();
                String filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                b8 isExist = FALSE;
                if (m_impl->data.checkExistance != nullptr)
                {
                    isExist = m_impl->data.checkExistance(fileName);
                }

                if (isExist)
                {
                    ImGui::OpenPopup(m_impl->data.id.RawString().c_str());
                    m_impl->tempFilePath = fileName;
                    ImGuiFileDialog::Instance()->Close();
                }
                else
                {
                    m_impl->project->name = GetFileName(fileName);
                    m_impl->project->path = filePath;
                    m_impl->config->lastProjectPath = filePath;

                    if (m_impl->data.onOk != nullptr)
                    {
                        m_impl->data.onOk(FALSE);
                    }
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGuiFileDialog::Instance()->Close();
        }

        if (m_impl->data.checkExistance == nullptr)
        {
            return;
        }

        if (ImGui::BeginPopupModal(m_impl->data.id.RawString().c_str(),
                                   NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Project is already exist, do you want to overwrite it?");

            if (ImGui::Button("Yes", ImVec2(120, 0)))
            {
                m_impl->project->name = GetFileName(m_impl->tempFilePath);
                m_impl->project->path = GetFileFolder(m_impl->tempFilePath);
                m_impl->config->lastProjectPath = GetFileFolder(m_impl->tempFilePath);

                if (m_impl->data.onOk != nullptr)
                {
                    m_impl->data.onOk(TRUE);
                }

                ImGui::CloseCurrentPopup();
            }

            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("No", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
} // namespace ntt
