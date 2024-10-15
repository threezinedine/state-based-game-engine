#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/ecs/data_com.hpp>
#include "scriptable.hpp"
#include "script_store.hpp"
#include <NTTEngine/renderer/Hovering.hpp>

namespace ntt::script
{
    struct NativeScriptComponent : public ComponentBase
    {
        String scriptName;
        script_object_id_t objId = INVALID_OBJECT_ID;
        void *data;

        NativeScriptComponent(String scriptName = "",
                              script_object_id_t objId = INVALID_OBJECT_ID,
                              void *data = nullptr)
            : scriptName(scriptName), objId(objId),
              data(data)
        {
        }

        resource_id_t GetScriptId() const;
        Ref<Script> GetObj();
        String GetName() const override;

        JSON ToJSON() const override;
        void FromJSON(const JSON &j) override;

        void OnEditorUpdate(std::function<void()> callback, void *data) override;
    };
}