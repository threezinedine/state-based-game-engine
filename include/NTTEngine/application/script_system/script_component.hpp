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
        resource_id_t scriptId = INVALID_SCRIPT_ID;
        script_object_id_t objId = INVALID_OBJECT_ID;

        NativeScriptComponent(resource_id_t scriptId,
                              script_object_id_t objId = INVALID_OBJECT_ID)
            : scriptId(scriptId), objId(objId) {}

        Ref<Script> GetObj()
        {
            if (scriptId == INVALID_SCRIPT_ID)
            {
                return nullptr;
            }

            if (objId == INVALID_OBJECT_ID)
            {
                return nullptr;
            }

            return GET_SCRIPT(Script, objId);
        }
    };
}