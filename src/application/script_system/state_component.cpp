#include <NTTEngine/application/script_system/state_component.hpp>
#include "imgui.h"
#include <NTTEngine/editor/types.hpp>

namespace ntt
{
    String StateComponent::GetName() const
    {
        return "StateComponent";
    }

    JSON StateComponent::ToJSON() const
    {
        JSON json("{}");
        json.Set("defaultState", defaultState);

        JSON stateScriptNamesJson("{}");

        for (auto &pair : stateScriptNames)
        {
            stateScriptNamesJson.Set(pair.first, pair.second);
        }

        json.Set("stateScriptNames", stateScriptNamesJson);

        return json;
    }

    void StateComponent::FromJSON(const JSON &json)
    {
        defaultState = json.Get<String>("defaultState");

        JSON stateScriptNamesJson = json.Get<JSON>("stateScriptNames");

        for (auto key : stateScriptNamesJson.GetKeys())
        {
            stateScriptNames[key] = stateScriptNamesJson.Get<String>(key);
        }
    }

    void StateComponent::OnEditorUpdate(std::function<void()> onChanged, void *data)
    {
        if (ImGui::BeginCombo("Default State", defaultState.RawString().c_str()))
        {
            for (auto &pair : stateScriptNames)
            {
                b8 isSelected = defaultState == pair.first;

                if (ImGui::Selectable(pair.first.RawString().c_str(), isSelected))
                {
                    defaultState = pair.first;

                    if (onChanged)
                    {
                        onChanged();
                    }
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::TreeNode("States"))
        {
            for (auto &pair : stateScriptNames)
            {
                ImGui::Text("State: %s", pair.first.RawString().c_str());
            }

            ImGui::TreePop();
        }

        if (ImGui::Button("Add state"))
        {
            ImGui::OpenPopup("AddState");
        }

        if (ImGui::BeginPopup("AddState"))
        {
            if (data == nullptr)
            {
                ImGui::CloseCurrentPopup();
                return;
            }

            EditorData *editorData = (EditorData *)data;

            if (editorData == nullptr)
            {
                ImGui::CloseCurrentPopup();
                return;
            }

            List<String> scriptNames;

            for (auto resource : editorData->project->defaultResources)
            {
                if (resource.type == ResourceType::SCRIPT)
                {
                    auto scriptId = GetScriptIdByName(resource.name);

                    if (ScriptStoreGetBaseType(scriptId) == typeid(State))
                    {
                        scriptNames.push_back(resource.name);
                    }
                }
            }

            for (auto resource : editorData->scene->resources)
            {
                if (resource.type == ResourceType::SCRIPT)
                {
                    auto scriptId = GetScriptIdByName(resource.name);

                    if (ScriptStoreGetBaseType(scriptId) == typeid(State))
                    {
                        scriptNames.push_back(resource.name);
                    }
                }
            }

            static char stateName[256] = {0};
            static u8 scriptIndex = 0;

            if (scriptNames.size() == 0)
            {
                ImGui::Text("No script found");
                ImGui::EndPopup();
            }
            else
            {
                ImGui::InputText("State name", stateName, 256);

                if (ImGui::BeginCombo("State", scriptNames[scriptIndex].RawString().c_str()))
                {
                    for (auto i = 0; i < scriptNames.size(); i++)
                    {
                        b8 isSelected = scriptIndex == i;

                        if (ImGui::Selectable(scriptNames[i].RawString().c_str(), isSelected))
                        {
                            scriptIndex = i;
                        }
                    }

                    ImGui::EndCombo();
                }

                if (ImGui::Button("Cancel"))
                {
                    memset(stateName, 0, sizeof(stateName));
                    scriptIndex = 0;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Save"))
                {
                    stateScriptNames[stateName] = scriptNames[scriptIndex];

                    if (onChanged)
                    {
                        onChanged();
                    }

                    memset(stateName, 0, sizeof(stateName));
                    scriptIndex = 0;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }
} // namespace ntt
