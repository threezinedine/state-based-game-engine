#include <NTTEngine/application/script_system/native_system.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/dictionary.hpp>

namespace ntt::script
{
    using namespace ecs;
    using namespace log;
    using namespace memory;

    namespace
    {
        struct ScriptLifetime
        {
            std::function<void()> OnCreate;
            std::function<void()> OnDestroy;
            std::function<void(f32)> OnUpdate;
        };

        b8 s_initialized = FALSE;

        Dictionary<entity_id_t, ScriptLifetime> s_scripts;
    } // namespace

    void Script::OnEnter()
    {
        OnEnterImpl();
    }

    void Script::OnExit()
    {
        OnExitImpl();
    }

    void Script::OnUpdate(f32 deltaTime)
    {
        OnUpdateImpl(deltaTime);
    }

    void ScriptInit()
    {
        if (s_initialized)
        {
            NTT_ENGINE_WARN("The Script system is already initialized");
            return;
        }

        s_initialized = TRUE;
    }

    void ScriptInitFunc(entity_id_t entity_id)
    {
        auto script = ECS_GET_COMPONENT(entity_id, NativeScriptComponent);

        if (script == nullptr)
        {
            NTT_ENGINE_WARN("The Script component is not found");
            return;
        }

        script->ins->SetEntity(entity_id);
        script->ins->OnEnter();
    }

    void ScriptUpdate(f32 deltaTime, entity_id_t entity_id)
    {
        auto script = ECS_GET_COMPONENT(entity_id, NativeScriptComponent);

        if (script == nullptr)
        {
            NTT_ENGINE_WARN("The Script component is not found");
            return;
        }

        script->ins->OnUpdate(deltaTime);
    }

    void ScriptShutdownFunc(entity_id_t entity_id)
    {
        auto script = ECS_GET_COMPONENT(entity_id, NativeScriptComponent);

        if (script == nullptr)
        {
            NTT_ENGINE_WARN("The Script component is not found");
            return;
        }

        script->ins->OnExit();
    }

    void ScriptShutdown()
    {
        if (!s_initialized)
        {
            NTT_ENGINE_WARN("The Script system is already shutdown");
            return;
        }

        s_initialized = FALSE;
    }
} // namespace ntt::script
