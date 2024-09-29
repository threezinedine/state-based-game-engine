#include <NTTEngine/application/script_system/native_script_system.hpp>
#include <NTTEngine/application/script_system/native_script.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt::script
{
    using namespace ecs;
    using namespace log;
    using namespace memory;

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

        if (script->ins == nullptr)
        {
            NTT_ENGINE_WARN("The Script instance is not found");
            return;
        }

        script->ins->SetEntity(entity_id);
        script->ins->OnEnter();
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

        if (script->ins == nullptr)
        {
            NTT_ENGINE_WARN("The Script instance is not found");
            return;
        }

        script->ins->OnUpdate(deltaTime);
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

        if (script->ins == nullptr)
        {
            NTT_ENGINE_WARN("The Script instance is not found");
            return;
        }

        script->ins->OnExit();
    }

    void ScriptSystem::ShutdownSystem()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt::script
