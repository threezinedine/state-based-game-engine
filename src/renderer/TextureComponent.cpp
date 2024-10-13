#include <NTTEngine/renderer/TextureComponent.hpp>
#include "imgui.h"
#include <NTTEngine/resources/ResourceManager.hpp>
#include <NTTEngine/editor/types.hpp>

namespace ntt::renderer
{

    resource_id_t TextureComponent::GetTextureID() const
    {
        return GetResourceID(resourceName);
    }

    String TextureComponent::ResourceName() const
    {
        return resourceName;
    }

    String TextureComponent::GetName() const
    {
        return "TextureComponent";
    }

    JSON TextureComponent::ToJSON() const
    {
        JSON json;

        json.Set("resource_name", ResourceName());
        json.Set("current_cell", currentCell.ToJSON());
        json.Set("texture_grid", textureGrid.ToJSON());
        json.Set("tooltip", tooltip);

        return json;
    }

    void TextureComponent::FromJSON(const JSON &json)
    {
        resourceName = json.Get<String>("resource_name");
        currentCell.FromJSON(json.Get<JSON>("current_cell"));
        textureGrid.FromJSON(json.Get<JSON>("texture_grid"));
        tooltip = json.Get<String>("tooltip");
    }

    void TextureComponent::OnEditorUpdate(std::function<void()> onChanged, void *data)
    {

        ImGui::Text(format("Texture: {} - Id: {}",
                           ResourceName(), GetTextureID())
                        .RawString()
                        .c_str());
        if (ImGui::Button("Select"))
        {
            ImGui::OpenPopup("SelectTexture");
        }
        ImGui::Separator();

        ImGui::Text("Current Cell:");
        currentCell.EditorUpdate(onChanged);
        ImGui::Text(format("Tooltip: {}", tooltip).RawString().c_str());

        if (ImGui::BeginPopupModal("SelectTexture"))
        {
            EditorData *editorData = (EditorData *)data;
            List<String> resourceNames = {};

            for (auto &info : editorData->scene->GetResourceInfo())
            {
                if (info.type != IMAGE)
                {
                    continue;
                }
                resourceNames.push_back(info.name);
            }

            for (auto &info : editorData->project->GetDefaultResourcesInfo())
            {
                if (info.type != IMAGE)
                {
                    continue;
                }
                resourceNames.push_back(info.name);
            }

            if (ImGui::BeginCombo("Texture", ResourceName().RawString().c_str()))
            {
                for (auto &resourceName : resourceNames)
                {
                    b8 isSelected = resourceName == ResourceName();
                    if (ImGui::Selectable(resourceName.RawString().c_str(), isSelected))
                    {
                        this->resourceName = resourceName;
                        if (onChanged != nullptr)
                        {
                            onChanged();
                        }
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
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Save"))
            {
                if (onChanged != nullptr)
                {
                    onChanged();
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
} // namespace ntt::renderer
