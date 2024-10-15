#include <NTTEngine/application/script_system/script_component.hpp>
#include "imgui.h"
#include <NTTEngine/editor/types.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    resource_id_t NativeScriptComponent::GetScriptId() const
    {
        PROFILE_FUNCTION();
        return GetScriptIdByName(scriptName);
    }

    Ref<Script> NativeScriptComponent::GetObj()
    {
        PROFILE_FUNCTION();

        if (GetScriptId() == INVALID_SCRIPT_ID)
        {
            return nullptr;
        }

        if (objId == INVALID_OBJECT_ID)
        {
            return nullptr;
        }

        return GET_SCRIPT(Script, objId);
    }

    String NativeScriptComponent::GetName() const
    {
        PROFILE_FUNCTION();
        return "NativeScriptComponent";
    }

    JSON NativeScriptComponent::ToJSON() const
    {
        PROFILE_FUNCTION();
        JSON json;
        json.Set<String>("scriptName", scriptName);
        return json;
    }

    void NativeScriptComponent::FromJSON(const JSON &j)
    {
        PROFILE_FUNCTION();
        scriptName = j.Get<String>("scriptName");
    }

    void NativeScriptComponent::OnEditorUpdate(std::function<void()> onChanged, void *data)
    {
        PROFILE_FUNCTION();
        EditorData *editorData;
        static u8 scriptIndex = 255;

        if (data == nullptr)
        {
            return;
        }

        editorData = static_cast<EditorData *>(data);

        List<String> scriptNames;

        for (auto resource : editorData->project->defaultResources)
        {
            if (resource.type == ResourceType::SCRIPT)
            {
                scriptNames.push_back(resource.name);
            }
        }

        for (auto resource : editorData->scene->resources)
        {
            if (resource.type == ResourceType::SCRIPT)
            {
                scriptNames.push_back(resource.name);
            }
        }

        if (scriptNames.size() == 0)
        {
            ImGui::Text("No script found");
            return;
        }

        for (auto i = 0; i < scriptNames.size(); i++)
        {
            if (scriptName == scriptNames[i])
            {
                scriptIndex = i;
                break;
            }
        }

        if (ImGui::BeginCombo(
                "Script",
                scriptIndex == 255 ? "" : scriptNames[scriptIndex].RawString().c_str()))
        {
            for (auto i = 0; i < scriptNames.size(); i++)
            {
                const b8 isSelected = scriptIndex == i;
                if (ImGui::Selectable(scriptNames[i].RawString().c_str(), isSelected))
                {
                    scriptIndex = i;
                    scriptName = scriptNames[i];
                    if (onChanged)
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
    }
} // namespace ntt
