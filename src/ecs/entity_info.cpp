#include <NTTEngine/ecs/entity_info.hpp>
#include <NTTEngine/ecs/ecs_helper.hpp>
#include "imgui.h"
#include <array>
#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/core/logging/logging.hpp>

namespace ntt::ecs
{
    using namespace renderer;
    using namespace log;

    namespace
    {
        std::array<const char *, 1> componentTypes = {"Geometry"};
        u8 selectedComponentType = 0;
    }

    void EntityInfo::FromJSON(const JSON &entity)
    {
        name = entity.Get<String>("name");

        JSON componentsCfg = entity.Get<JSON>("components");
        components = ECS_From(componentsCfg);
    }

    JSON EntityInfo::ToJSON() const
    {
        JSON entity = JSON("{}");
        entity.Set("name", name);
        entity.Set("components", ECS_ToJSON(components));

        return entity;
    }

    void EntityInfo::OnEditorUpdate(std::function<void()> onChanged)
    {
        static char tempName[256] = {0};

        if (strlen(tempName) == 0)
        {
            memcpy(tempName, name.RawString().c_str(), name.Length());
        }

        ImGui::Text("Name: %s", name.RawString().c_str());
        ImGui::SameLine();

        if (ImGui::Button("Rename"))
        {
            ImGui::OpenPopup("RenameEntity");
        }
        ImGui::Separator();
        if (ImGui::Button("Add component"))
        {
            ImGui::OpenPopup("add_component");
        }
        ImGui::Separator();

        if (ImGui::TreeNode("Components"))
        {
            ImGui::SetNextItemOpen(TRUE, ImGuiCond_Once);
            for (auto &component : components)
            {
                component.second->OnEditorUpdate(onChanged);
            }

            ImGui::TreePop();
        }

        if (ImGui::BeginPopup("RenameEntity"))
        {
            ImGui::InputText(
                "Name",
                tempName,
                20);

            if (ImGui::Button("Save"))
            {
                name = tempName;
                NTT_ENGINE_DEBUG("Entity name: {}", name);
                memset(tempName, 0, sizeof(tempName));
                if (onChanged)
                {
                    onChanged();
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();

            if (ImGui::BeginPopupModal("add_component"))
            {
                if (ImGui::BeginCombo(
                        "Component",
                        componentTypes[selectedComponentType]))
                {
                    for (u8 i = 0; i < componentTypes.size(); i++)
                    {
                        b8 isSelected = selectedComponentType == i;
                        if (ImGui::Selectable(componentTypes[i], isSelected))
                        {
                            selectedComponentType = i;
                        }

                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                b8 componentExisted = components.Contains(typeid(Geometry));

                ImGui::Separator();
                if (ImGui::Button("Cancel"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();

                ImGui::BeginDisabled(componentExisted);
                if (ImGui::Button("Save"))
                {
                    components[typeid(Geometry)] = CreateRef<Geometry>();
                    selectedComponentType = 0;

                    if (onChanged)
                    {
                        onChanged();
                    }

                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndDisabled();
                ImGui::EndPopup();
            }
        }
    }
} // namespace ntt
