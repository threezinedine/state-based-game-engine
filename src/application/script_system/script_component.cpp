#include <NTTEngine/application/script_system/script_component.hpp>
#include "imgui.h"

namespace ntt::script
{
    resource_id_t NativeScriptComponent::GetScriptId() const
    {
        return GetScriptIdByName(scriptName);
    }

    Ref<Script> NativeScriptComponent::GetObj()
    {
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
        return "NativeScriptComponent";
    }

    JSON NativeScriptComponent::ToJSON() const
    {
        JSON json;
        json.Set<String>("scriptName", "");
        return json;
    }

    void NativeScriptComponent::FromJSON(const JSON &j)
    {
    }

    void NativeScriptComponent::OnEditorUpdate(std::function<void()> callback, void *data)
    {
    }
} // namespace ntt
