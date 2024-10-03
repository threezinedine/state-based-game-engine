#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/ecs/data_com.hpp>
#include "scriptable.hpp"
#include "script_store.hpp"
#include <NTTEngine/renderer/Hovering.hpp>

namespace ntt::script
{
    using namespace ecs;
    using namespace renderer;

    struct NativeScriptComponent;

    class Script : public Scriptable
    {
    public:
        virtual void OnEnter();
        virtual void OnExit();
        void OnUpdate(f32 deltaTime);

    protected:
        virtual void OnUpdateImpl(f32 deltaTime) {}
        virtual void OnHoverEnter() {}
        virtual void OnHover(HoveringContext &context) {}
        virtual void OnHoverExit() {}
        virtual void OnCollide(List<entity_id_t> others) {}
    };

    struct NativeScriptComponent : public ComponentBase
    {
        resource_id_t scriptId = INVALID_SCRIPT_ID;
        script_object_id_t objId = INVALID_OBJECT_ID;

        NativeScriptComponent(resource_id_t scriptId,
                              script_object_id_t objId = INVALID_OBJECT_ID)
            : scriptId(scriptId), objId(objId) {}

        Script *GetObj()
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
} // namespace ntt::script
