#include "resource_window.hpp"
#include "imgui.h"
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/resources/resources.hpp>
#include "ImGuiFileDialog.h"
#include "image_window.hpp"
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/audio/audio.hpp>
#include <NTTEngine/application/script_system/script_store.hpp>

namespace ntt
{
    class ResourceWindow::Impl
    {
    public:
        Ref<ProjectInfo> project;
        Ref<EditorConfig> config;
        Ref<SceneInfo> scene;
        EditorData data;

        char resourceName[128];
        String resourcePath;
        u32 currentTypeIndex;
        b8 createInDefaultResource;

        const char *resourceTypes[3] = {"Image", "Audio", "Script"};
        const char *extensions[3] = {".png", ".wav", ".cpp"};

        Scope<ImageWindow> imageWindow;

        void ComponentImageDraw(ResourceInfo &info)
        {
            static i32 numCol = 1, numRow = 1;
            Grid grid{1, 1};
            grid.FromJSON(info.addintionalInfo.Get<JSON>("grid"));

            grid.row = grid.row != 0 ? grid.row : 1;
            grid.col = grid.col != 0 ? grid.col : 1;

            grid.EditorUpdate(
                [&]()
                {
                if (imageWindow != nullptr)
                {
                    imageWindow->ChangeGrid(grid.row, grid.col);
                }
                info.addintionalInfo.Set("grid", grid.ToJSON());
                Save(); });

            if (ImGui::Button(format("Show", info.name).RawString().c_str()))
            {
                if (imageWindow == nullptr)
                {
                    imageWindow = CreateScope<ImageWindow>(info.path, grid.row, grid.col);
                    imageWindow->Init();
                }
                else
                {
                    imageWindow->ChangeImage(info.path, grid.row, grid.col);
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

        void ComponentScriptDraw(ResourceInfo &info)
        {
        }

        b8 HasScene()
        {
            return project->ContainScene(scene);
        }

        void Save()
        {
            ResourceUnload(scene->resources);
            ResourceUnload(project->defaultResources);
            project->SaveDefaultResources();

            if (HasScene())
            {
                scene->SaveResourceInfo();
            }

            ResourceLoad(project->defaultResources);
            ResourceLoad(scene->resources);
        }

        void EditorWindowDraw(ResourceInfo &info, b8 useScene = FALSE)
        {
            info.OnEditorUpdate(
                [&]()
                { Save(); },
                &data);

            switch (info.type)
            {
            case ResourceType::IMAGE:
                ComponentImageDraw(info);
                break;
            case ResourceType::AUDIO:
                ComponentAudioDraw(info);
                break;
            case ResourceType::SCRIPT:
                ComponentScriptDraw(info);
                break;
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete"))
            {
                ResourceUnload({info});

                if (useScene)
                {
                    scene->resources.RemoveItem(
                        info,
                        [](const ResourceInfo &a, const ResourceInfo &b) -> b8
                        { return a.name == b.name; });
                }
                else
                {
                    project->defaultResources.RemoveItem(
                        info,
                        [](const ResourceInfo &a, const ResourceInfo &b) -> b8
                        {
                            return a.name == b.name;
                        });
                }

                Save();
            }

            if (ImGui::BeginPopup("rename"))
            {
                ImGui::InputText("New Name", resourceName, sizeof(resourceName));
                if (ImGui::Button("Rename"))
                {
                    info.name = resourceName;
                    Save();
                    memset(resourceName, 0, sizeof(resourceName));
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGuiFileDialog::Instance()->Display("changeFile"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    String fileName = ImGuiFileDialog::Instance()->GetFilePathName();
                    String filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                    info.path = fileName;
                    Save();
                }

                ImGuiFileDialog::Instance()->Close();
            }
        }

        void ResourceInit()
        {
            ResourceUnload(project->defaultResources);
            project->ReloadDefaultResourcesInfo();
            ResourceLoad(project->defaultResources);

            if (HasScene())
            {
                ResourceUnload(scene->resources);
                scene->ReloadResourceInfo();
                ResourceLoad(scene->resources);
            }
        }
    };

    ResourceWindow::ResourceWindow(Ref<ProjectInfo> project,
                                   Ref<EditorConfig> config,
                                   Ref<SceneInfo> scene)
        : OpenClosableWindow("Resource")
    {
        m_impl = CreateScope<Impl>();

        m_impl->project = project;
        m_impl->config = config;
        m_impl->scene = scene;

        memset(m_impl->resourceName, 0, sizeof(m_impl->resourceName));
        m_impl->currentTypeIndex = 0;
        m_impl->resourcePath = "";

        m_impl->data.project = project;
        m_impl->data.config = config;
        m_impl->data.scene = scene;

        m_impl->ResourceInit();
    }

    ResourceWindow::~ResourceWindow() {}

    void ResourceWindow::InitImpl()
    {
    }

    void ResourceWindow::OnReloadProject()
    {
        m_impl->ResourceInit();
    }

    void ResourceWindow::OnReloadScene()
    {
        m_impl->ResourceInit();
    }

    void ResourceWindow::UpdateImpl(b8 *p_open, ImGuiWindowFlags flags)
    {
        if (m_impl->imageWindow != nullptr)
        {
            m_impl->imageWindow->Update(flags);
        }

        if (ImGui::Begin("Resource", p_open))
        {
            ImGui::Text("Resource Window");
            ImGui::Separator();

            ImGui::Separator();
            if (ImGui::Button("New"))
            {
                ImGui::OpenPopup("newResource");
            }
            ImGui::SameLine();
            if (ImGui::Button("Save"))
            {
                ResourceUnload(m_impl->scene->resources);
                ResourceUnload(m_impl->project->defaultResources);
                m_impl->project->SaveDefaultResources();

                if (m_impl->HasScene())
                {
                    m_impl->scene->SaveResourceInfo();
                }

                ResourceLoad(m_impl->project->defaultResources);
                ResourceLoad(m_impl->scene->resources);
            }
            ImGui::Separator();

            if (ImGui::TreeNode("Default"))
            {
                ImGui::SetNextItemOpen(TRUE, ImGuiCond_Once);
                for (auto &info : m_impl->project->defaultResources)
                {
                    ImGui::PushID(info.name.RawString().c_str());
                    if (ImGui::TreeNode(info.name.RawString().c_str()))
                    {
                        m_impl->EditorWindowDraw(info);
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }

                ImGui::TreePop();
            }

            if (m_impl->HasScene())
            {
                if (ImGui::TreeNode(m_impl->scene->sceneName.RawString().c_str()))
                {
                    for (auto &info : m_impl->scene->resources)
                    {
                        ImGui::PushID(info.name.RawString().c_str());
                        if (ImGui::TreeNode(info.name.RawString().c_str()))
                        {
                            m_impl->EditorWindowDraw(info, TRUE);
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }

                    ImGui::TreePop();
                }
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

            ImGui::BeginDisabled(m_impl->resourceTypes[m_impl->currentTypeIndex] != "Script");
            ImGui::SameLine();
            if (ImGui::Button("Create"))
            {
                IGFD::FileDialogConfig config;
                config.path = m_impl->project->path.RawString();

                ImGuiFileDialog::Instance()->OpenDialog(
                    "createScript",
                    "Place the script file",
                    ".cpp",
                    config);
            }
            ImGui::EndDisabled();

            if (m_impl->HasScene())
            {
                ImGui::Checkbox("Default resource", &m_impl->createInDefaultResource);
            }
            else
            {
                ImGui::Text("The resource will be added into the default resource");
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

                if (m_impl->createInDefaultResource || !m_impl->HasScene())
                {
                    m_impl->project->defaultResources.push_back(info);
                }
                else
                {
                    m_impl->scene->resources.push_back(info);
                }

                // load the new resource into the system
                ResourceLoad({info});

                m_impl->Save();

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

            if (ImGuiFileDialog::Instance()->Display("createScript"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    String fileName = ImGuiFileDialog::Instance()->GetFilePathName();
                    String filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                    m_impl->resourcePath = fileName;

                    String nameWithoutExt = GetFileWithoutExtension(fileName);
                    nameWithoutExt.Replace(" ", "_");
                    nameWithoutExt.Replace("-", "_");

                    OpenFile(fileName);
                    Write(format(R"(
#include <NTTEngine/NTTEngine.hpp>

class {} : public Script
{
public:
    {}(void *data)
    {
    }

    ~{}()
    {
    }
};

SCRIPT_DEFINE({}, Script);
                    )",
                                 nameWithoutExt,
                                 nameWithoutExt,
                                 nameWithoutExt,
                                 nameWithoutExt));
                    CloseFile();
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
