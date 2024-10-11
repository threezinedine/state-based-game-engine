#include "resource_window.hpp"
#include "imgui.h"
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/resources/resources.hpp>
#include "ImGuiFileDialog.h"
#include "image_window.hpp"
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/audio/audio.hpp>

#include "utils.hpp"

namespace ntt
{
    using namespace renderer;
    using namespace audio;

    class ResourceWindow::Impl
    {
    public:
        Ref<ProjectInfo> project;
        List<ResourceInfo> resources;
        Ref<EditorConfig> config;
        Grid grid;

        List<String> resourceFiles;
        u32 currentIndex;

        char resourceName[128];
        String resourcePath;
        u32 currentTypeIndex;

        const char *resourceTypes[3] = {"Image", "Audio", "Script"};
        const char *extensions[3] = {".png", ".wav", ".cpp"};

        Scope<ImageWindow> imageWindow;

        void ComponentImageDraw(ResourceInfo &info)
        {
            static i32 numCol = 1, numRow = 1;
            numCol = info.addintionalInfo.Get("numCol", 1);
            numRow = info.addintionalInfo.Get("numRow", 1);

            ImGui::InputInt(format("Num Rows", info.name).RawString().c_str(), &numRow, 1, 10);
            ImGui::InputInt(format("Num Cols", info.name).RawString().c_str(), &numCol, 1, 10);

            if (numCol != info.addintionalInfo.Get("numCol", 1) ||
                numRow != info.addintionalInfo.Get("numRow", 1))
            {
                info.addintionalInfo.Set("numCol", numCol);
                info.addintionalInfo.Set("numRow", numRow);

                if (imageWindow != nullptr)
                {
                    imageWindow->ChangeGrid(numCol, numRow);
                }
            }

            if (ImGui::Button(format("Show", info.name).RawString().c_str()))
            {
                if (imageWindow == nullptr)
                {
                    imageWindow = CreateScope<ImageWindow>(info.path, numRow, numCol);
                    imageWindow->Init();
                }
                else
                {
                    imageWindow->ChangeImage(info.path, numRow, numCol);
                }

                imageWindow->Open();
            }
        }

        void ComponentAudioDraw(ResourceInfo &info)
        {
            if (ImGui::Button("Play"))
            {
                PlayAudio(GetResourceID(info.name));
            }
            ImGui::SameLine();
            if (ImGui::Button("Stop"))
            {
                StopAudio(GetResourceID(info.name));
            }
        }

        void EditorWindowDraw(ResourceInfo &info)
        {
            auto path = SubtractPath(info.path, project->path);

            ImGui::Text("Name: %s", info.name.RawString().c_str());
            ImGui::Text("Type: %s", resourceTypes[static_cast<u32>(info.type)]);
            ImGui::Text("Path: %s", path.RawString().c_str());

            switch (info.type)
            {
            case ResourceType::IMAGE:
                ComponentImageDraw(info);
                break;
            case ResourceType::AUDIO:
                ComponentAudioDraw(info);
                break;
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete"))
            {
                ResourceUnload({info});

                resources.RemoveItem(info, [](const ResourceInfo &a, const ResourceInfo &b) -> b8
                                     { return a.name == b.name; });

                SaveResourceFile(
                    JoinPath({
                        project->path,
                        project->defaultResourceFile,
                    }),
                    resources);
            }
        }
    };

    ResourceWindow::ResourceWindow(Ref<ProjectInfo> project, Ref<EditorConfig> config)
        : OpenClosableWindow("Resource")
    {
        m_impl = CreateScope<Impl>();

        m_impl->project = project;
        m_impl->config = config;

        m_impl->resourceFiles.clear();
        m_impl->resourceFiles.push_back(GetFileWithoutExtension("default_resource.json"));
        m_impl->currentIndex = 0;

        memset(m_impl->resourceName, 0, sizeof(m_impl->resourceName));
        m_impl->currentTypeIndex = 0;
        m_impl->resourcePath = "";

        m_impl->resources.clear();

        auto defaultConfigFile =
            JoinPath({m_impl->project->path, m_impl->project->defaultResourceFile});

        if (IsExist(defaultConfigFile))
        {
            JSON config(ReadFile(defaultConfigFile));

            auto resources = ExtractInfoFromJSON(config);

            for (auto &resource : resources)
            {
                m_impl->resources.push_back(resource);
            }
        }

        ResourceLoad(m_impl->resources);
    }

    ResourceWindow::~ResourceWindow() {}

    void ResourceWindow::InitImpl()
    {
    }

    void ResourceWindow::OnReloadProject()
    {
        auto defaultConfigFile =
            JoinPath({m_impl->project->path, m_impl->project->defaultResourceFile});

        m_impl->resources.clear();
        if (IsExist(defaultConfigFile))
        {
            JSON config(ReadFile(defaultConfigFile));

            auto resources = ExtractInfoFromJSON(config);

            for (auto &resource : resources)
            {
                m_impl->resources.push_back(resource);
            }
        }
    }

    void ResourceWindow::UpdateImpl(b8 *p_open)
    {
        if (m_impl->imageWindow != nullptr)
        {
            m_impl->imageWindow->Update();
        }

        if (ImGui::Begin("Resource", p_open))
        {
            ImGui::Text("Resource Window");
            ImGui::Separator();

            const char *current_item = m_impl->resourceFiles[m_impl->currentIndex].RawString().c_str();
            if (ImGui::BeginCombo("Resource", current_item))
            {
                for (u32 i = 0; i < m_impl->resourceFiles.size(); ++i)
                {
                    bool is_selected = (m_impl->currentIndex == i);
                    if (ImGui::Selectable(m_impl->resourceFiles[i].RawString().c_str(), is_selected))
                    {
                        m_impl->currentIndex = i;
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Separator();
            if (ImGui::Button("New"))
            {
                ImGui::OpenPopup("newResource");
            }
            ImGui::SameLine();
            if (ImGui::Button("Save"))
            {
                ResourceUnload(m_impl->resources);

                SaveResourceFile(
                    JoinPath({
                        m_impl->project->path,
                        m_impl->project->defaultResourceFile,
                    }),
                    m_impl->resources);

                ResourceLoad(m_impl->resources);
            }
            ImGui::Separator();

            ImGui::SetNextItemOpen(TRUE, ImGuiCond_Once);
            for (auto &info : m_impl->resources)
            {
                ImGui::PushID(info.name.RawString().c_str());
                if (ImGui::TreeNode(info.name.RawString().c_str()))
                {
                    m_impl->EditorWindowDraw(info);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("newResource", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("New Resource");
            ImGui::InputText("Resource Name", m_impl->resourceName, sizeof(m_impl->resourceName));

            if (ImGui::BeginCombo(
                    "Type",
                    m_impl->resourceTypes[m_impl->currentTypeIndex]))
            {
                for (u32 i = 0; i < 3; ++i)
                {
                    bool is_selected = (m_impl->currentTypeIndex == i);
                    if (ImGui::Selectable(m_impl->resourceTypes[i], is_selected))
                    {
                        m_impl->currentTypeIndex = i;
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::Text(
                format("Path: {}",
                       SubtractPath(m_impl->resourcePath, m_impl->project->path))
                    .RawString()
                    .c_str());
            ImGui::SameLine();
            if (ImGui::Button("..."))
            {
                IGFD::FileDialogConfig config;
                config.path = m_impl->project->path.RawString();

                ImGuiFileDialog::Instance()->OpenDialog(
                    "choosePath",
                    "Choose File",
                    m_impl->extensions[m_impl->currentTypeIndex],
                    config);
            }
            ImGui::Separator();

            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            b8 canSubmit = TRUE;

            if (strlen(m_impl->resourceName) == 0)
            {
                canSubmit = FALSE;
            }

            if (m_impl->resourcePath.Length() == 0)
            {
                canSubmit = FALSE;
            }

            ImGui::BeginDisabled(!canSubmit);
            if (ImGui::Button("OK"))
            {
                ResourceInfo info;
                info.name = m_impl->resourceName;
                info.path = m_impl->resourcePath;
                info.type = static_cast<ResourceType>(m_impl->currentTypeIndex);

                memset(m_impl->resourceName, 0, sizeof(m_impl->resourceName));
                m_impl->resourcePath = "";
                m_impl->currentTypeIndex = 0;

                m_impl->resources.push_back(info);

                // load the new resource into the system
                ResourceLoad({info});

                SaveResourceFile(
                    JoinPath({
                        m_impl->project->path,
                        m_impl->project->defaultResourceFile,
                    }),
                    m_impl->resources);

                ImGui::CloseCurrentPopup();
            }
            ImGui::EndDisabled();

            if (ImGuiFileDialog::Instance()->Display("choosePath"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    String fileName = ImGuiFileDialog::Instance()->GetFilePathName();
                    String filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                    m_impl->resourcePath = fileName;
                }

                ImGuiFileDialog::Instance()->Close();
            }

            ImGui::EndPopup();
        }
        ImGui::End();
    }

    void ResourceWindow::ShutdownImpl()
    {
        if (m_impl->imageWindow != nullptr)
        {
            m_impl->imageWindow->Shutdown();
            m_impl->imageWindow.reset();
        }
    }
} // namespace ntt
