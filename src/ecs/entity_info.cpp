#include <NTTEngine/ecs/entity_info.hpp>
#include <NTTEngine/ecs/ecs_helper.hpp>
#include "imgui.h"
#include <array>
#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/renderer/TextureComponent.hpp>
#include <NTTEngine/physics/Mass.hpp>
#include <NTTEngine/renderer/Sprite.hpp>
#include <NTTEngine/application/script_system/script_component.hpp>
#include <NTTEngine/application/script_system/state_component.hpp>
#include <NTTEngine/renderer/Hovering.hpp>
#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/physics/collision.hpp>

namespace ntt
{
    namespace
    {
        List<String> componentTypes =
            {
                "Geometry",
                "TextureCompontent",
                "Mass",
                "Sprite",
                "NativeScriptComponent",
                "Hovering",
                "StateComponent",
                "Collision"};

        List<std::type_index> componentIndexes =
            {
                typeid(Geometry),
                typeid(TextureComponent),
                typeid(Mass),
                typeid(Sprite),
                typeid(NativeScriptComponent),
                typeid(Hovering),
                typeid(StateComponent),
                typeid(Collision)};
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

    void EntityInfo::OnEditorUpdate(std::function<void()> onChanged, void *data)
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
            b8 modified = FALSE;
            for (auto &component : components)
            {
                ImGui::PushID(format("{}_{}", name, component.second->GetName()).RawString().c_str());
                if (ImGui::TreeNode(component.second->GetName().RawString().c_str()))
                {
                    component.second->OnEditorUpdate([&]()
                                                     { modified = TRUE; }, data);
                    if (ImGui::Checkbox("Activate", &component.second->active))
                    {
                        component.second->active
                            ? component.second->TurnOn()
                            : component.second->TurnOff();
                    }

                    ImGui::TreePop();
                }

                ImGui::PopID();
            }

            if (modified && onChanged)
            {
                onChanged();
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
                Ref<EntityInfo> entity;

                for (auto &entityInfo : ((EditorData *)data)->scene->entities)
                {
                    if (entityInfo->name == name)
                    {
                        entity = entityInfo;
                        break;
                    }
                }

                entity->name = tempName;
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
        }

        if (ImGui::BeginPopupModal("add_component"))
        {
            if (ImGui::BeginCombo(
                    "Component",
                    componentTypes[selectedComponentType].RawString().c_str()))
            {
                for (u8 i = 0; i < componentTypes.size(); i++)
                {
                    if (components.Contains(componentIndexes[i]))
                    {
                        continue;
                    }
                    b8 isSelected = selectedComponentType == i;
                    if (ImGui::Selectable(componentTypes[i].RawString().c_str(), isSelected))
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

            std::type_index type = componentIndexes[selectedComponentType];
            b8 componentExisted = components.Contains(type);

            ImGui::Separator();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();

            ImGui::BeginDisabled(componentExisted);
            if (ImGui::Button("Save"))
            {
                EditorData *editorData = (EditorData *)data;

                Ref<EntityInfo> entity;

                for (auto &entityInfo : editorData->scene->entities)
                {
                    if (entityInfo->name == name)
                    {
                        entity = entityInfo;
                        break;
                    }
                }

                if (type == typeid(Geometry))
                {
                    entity->components[type] = CreateRef<Geometry>();
                }
                else if (type == typeid(TextureComponent))
                {
                    entity->components[type] = CreateRef<TextureComponent>();
                }
                else if (type == typeid(Mass))
                {
                    entity->components[type] = CreateRef<Mass>();
                }
                else if (type == typeid(Sprite))
                {
                    entity->components[type] = CreateRef<Sprite>();
                }
                else if (type == typeid(NativeScriptComponent))
                {
                    entity->components[type] = CreateRef<NativeScriptComponent>();
                }
                else if (type == typeid(Hovering))
                {
                    entity->components[type] = CreateRef<Hovering>();
                }
                else if (type == typeid(StateComponent))
                {
                    entity->components[type] = CreateRef<StateComponent>();
                }
                else if (type == typeid(Collision))
                {
                    entity->components[type] = CreateRef<Collision>();
                }

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
} // namespace ntt
