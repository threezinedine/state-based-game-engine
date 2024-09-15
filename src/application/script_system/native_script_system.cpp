#include <NTTEngine/application/script_system/native_system.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/structures/dictionary.hpp>

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
        m_impl = CreateScope<Impl>();
    }

    ScriptSystem::~ScriptSystem()
    {
    }

    void ScriptSystem::InitSystemImpl()
    {
    }

    void ScriptSystem::InitEntityImpl(entity_id_t entity_id)
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

    void ScriptSystem::UpdateImpl(f32 deltaTime, entity_id_t entity_id)
    {
        auto script = ECS_GET_COMPONENT(entity_id, NativeScriptComponent);

        if (script == nullptr)
        {
            NTT_ENGINE_WARN("The Script component is not found");
            return;
        }

        script->ins->OnUpdate(deltaTime);
    }

    void ScriptSystem::ShutdownEntityImpl(entity_id_t entity_id)
    {
        auto script = ECS_GET_COMPONENT(entity_id, NativeScriptComponent);

        if (script == nullptr)
        {
            NTT_ENGINE_WARN("The Script component is not found");
            return;
        }

        script->ins->OnExit();
    }

    void ScriptSystem::ShutdownSystemImpl()
    {
    }
} // namespace ntt::script
