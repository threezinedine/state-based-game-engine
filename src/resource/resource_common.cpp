#include <NTTEngine/resources/resource_common.h>
#include <NTTEngine/editor/types.hpp>
#include "imgui.h"
#include <NTTEngine/core/logging/logging.hpp>
#include "ImGuiFileDialog.h"
#include <NTTEngine/renderer/GraphicInterface.hpp>

namespace ntt
{
    using namespace log;
    using namespace renderer;

    namespace
    {
        const char *resourceTypes[3] = {"Image", "Audio", "Script"};
        const char *extensions[3] = {".png", ".wav", ".cpp"};
    }

    JSON ResourceInfo::ToJSON() const
    {
        JSON result;
        result.Set("name", name);
        result.Set("type", static_cast<u32>(type));
        result.Set("path", path);
        result.Set("extra", addintionalInfo);

        return result;
    }

    void ResourceInfo::From(const JSON &data)
    {
        name = data.Get<String>("name");
        type = static_cast<ResourceType>(data.Get<u32>("type"));
        path = data.Get<String>("path");
        addintionalInfo = data.Get<JSON>("extra");
    }

    void ResourceInfo::OnEditorUpdate(std::function<void()> onChanged, void *data)
    {
        static char resourceName[128] = {0};

        if (strlen(resourceName) == 0)
        {
            strcpy(resourceName, name.RawString().c_str());
        }

        EditorData *editorData = (EditorData *)data;

        if (editorData->project == nullptr)
        {
            return;
        }

        auto path = SubtractPath(this->path, editorData->project->path);

        ImGui::Text(format("Name: {}", name).RawString().c_str());

        ImGui::SameLine();
        if (ImGui::Button("Rename"))
        {
            ImGui::OpenPopup("rename");
        }

        ImGui::Text("Type: %s", resourceTypes[static_cast<u32>(type)]);

        ImGui::Text("Path: %s", path.RawString().c_str());
        ImGui::SameLine();
        if (ImGui::Button("Browse"))
        {
            IGFD::FileDialogConfig config;
            config.path = editorData->project->path.RawString();

            ImGuiFileDialog::Instance()->OpenDialog(
                "changeFile",
                "Choose file",
                extensions[static_cast<u32>(type)],
                config);
        }

        if (ImGui::BeginPopup("rename"))
        {
            ImGui::InputText("New Name", resourceName, sizeof(resourceName));
            if (ImGui::Button("Rename"))
            {
                name = resourceName;
                memset(resourceName, 0, sizeof(resourceName));
                if (onChanged != nullptr)
                {
                    onChanged();
                }
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

                path = fileName;

                if (onChanged != nullptr)
                {
                    onChanged();
                }
            }

            ImGuiFileDialog::Instance()->Close();
        }
    }
} // namespace ntt
