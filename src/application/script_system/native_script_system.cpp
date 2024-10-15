#include <NTTEngine/application/script_system/native_script_system.hpp>
#include <NTTEngine/application/script_system/native_script.hpp>
#include <NTTEngine/application/script_system/script_store.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/resources/ResourceManager.hpp>
#include <NTTEngine/application/script_system/script_component.hpp>

namespace ntt
{
#define THIS(exp) (m_impl->exp)

    class ScriptSystem::Impl
    {
    public:
    };

    ScriptSystem::ScriptSystem()
        : System()
    {
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();
    }

    ScriptSystem::~ScriptSystem()
    {
        PROFILE_FUNCTION();
    }

    void ScriptSystem::InitSystem()
    {
        PROFILE_FUNCTION();
    }

    void ScriptSystem::InitEntity(entity_id_t entity_id)
    {
        PROFILE_FUNCTION();
        auto script = ECS_GET_COMPONENT(entity_id, NativeScriptComponent);

        if (script == nullptr)
        {
            NTT_ENGINE_WARN("The Script component is not found");
            return;
        }

        if (script->GetScriptId() == INVALID_RESOURCE_ID)
        {
            NTT_ENGINE_TRACE("The Script resource id is not found");
            return;
        }

        script->objId = ScriptStoreCreate(script->GetScriptId(), script->data);
        auto obj = GET_SCRIPT(Script, script->objId);

        if (obj == nullptr)
        {
            ScriptStoreDeleteObject(script->objId);
            script->objId = INVALID_OBJECT_ID;
            return;
        }

        obj->SetEntity(entity_id);
        obj->OnEnter();
    }

    void ScriptSystem::Update(f32 deltaTime, entity_id_t entity_id)
    {
        PROFILE_FUNCTION();
        auto script = ECS_GET_COMPONENT(entity_id, NativeScriptComponent);

        if (script == nullptr)
        {
            NTT_ENGINE_WARN("The Script component is not found");
            return;
        }

        if (script->GetScriptId() == INVALID_RESOURCE_ID)
        {
            NTT_ENGINE_TRACE("The Script resource id is not found");
            return;
        }

        auto obj = script->GetObj();

        if (obj == nullptr)
        {
            script->objId = ScriptStoreCreate(script->GetScriptId(), script->data);
            obj = script->GetObj();

            if (obj == nullptr)
            {
                ScriptStoreDeleteObject(script->objId);
                script->objId = INVALID_OBJECT_ID;
                return;
            }

            obj->SetEntity(entity_id);
            obj->OnEnter();
        }

        obj->OnUpdate(deltaTime);
    }

    void ScriptSystem::ShutdownEntity(entity_id_t entity_id)
    {
        PROFILE_FUNCTION();
        auto script = ECS_GET_COMPONENT(entity_id, NativeScriptComponent);

        if (script == nullptr)
        {
            NTT_ENGINE_WARN("The Script component is not found");
            return;
        }

        auto obj = script->GetObj();

        if (obj == nullptr)
        {
            return;
        }

        obj->OnExit();
        ScriptStoreDeleteObject(script->objId);
        script->objId = INVALID_OBJECT_ID;
    }

    void ScriptSystem::ShutdownSystem()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt
