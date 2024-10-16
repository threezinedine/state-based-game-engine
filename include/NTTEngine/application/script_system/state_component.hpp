#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/component_base.hpp>
#include <NTTEngine/resources/resource_common.h>
#include "script_store.hpp"
#include "state.hpp"

namespace ntt
{

    struct StateComponent : public ComponentBase
    {
        Dictionary<String, String> stateScriptNames;
        String defaultState;
        List<script_object_id_t> stateObjIds;
        Ref<State> state;

        StateComponent(
            Dictionary<String, String> stateScriptNames = {},
            String defaultState = "")
            : stateScriptNames(stateScriptNames),
              defaultState(defaultState),
              state(nullptr)
        {
            stateObjIds = {};
        }

        String GetName() const override;

        JSON ToJSON() const override;
        void FromJSON(const JSON &json) override;

        void OnEditorUpdate(std::function<void()> onChanged = nullptr, void *data = nullptr) override;
    };

} // namespace ntt
