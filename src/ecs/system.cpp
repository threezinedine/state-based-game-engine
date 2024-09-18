#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/layer_system/layer_system.hpp>

#define THIS(exp) (m_impl->exp)

namespace ntt::ecs
{
    using namespace event;

    class System::Impl
    {
    public:
        b8 initialized = FALSE;
        b8 debugging = FALSE;

        event_id_t onDebugBreakEvent;
        event_id_t onDebugContinueEvent;
    };

    System::System()
    {
        m_impl = CreateScope<Impl>();

        RegisterEvent(NTT_DEBUG_BREAK, [this](...)
                      { THIS(debugging) = TRUE; });

        RegisterEvent(NTT_DEBUG_CONTINUE, [this](...)
                      { THIS(debugging) = FALSE; });
    }

    System::~System()
    {
    }

    void System::InitSystem()
    {
        PROFILE_FUNCTION();
        if (THIS(initialized) == TRUE)
        {
            NTT_ENGINE_WARN("The system is already initialized");
            return;
        }

        InitSystemImpl();

        THIS(initialized) = TRUE;
    }

    void System::InitEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
        if (THIS(initialized) == FALSE)
        {
            NTT_ENGINE_WARN("The system is not initialized");
            return;
        }

        InitEntityImpl(id);
    }

    b8 System::ShouldUpdate(entity_id_t id)
    {
        PROFILE_FUNCTION();
        return UpdatedEntities().Contains(id);
    }

    void System::Update(f32 delta, entity_id_t id)
    {
        PROFILE_FUNCTION();
        if (THIS(initialized) == FALSE)
        {
            NTT_ENGINE_WARN("The system is not initialized");
            return;
        }

        if (!ShouldUpdate(id))
        {
            return;
        }

        UpdateImpl(delta, id);
    }

    void System::ShutdownEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
        if (THIS(initialized) == FALSE)
        {
            NTT_ENGINE_WARN("The system is not initialized");
            return;
        }

        ShutdownEntityImpl(id);
    }

    void System::ShutdownSystem()
    {
        PROFILE_FUNCTION();
        if (THIS(initialized) == FALSE)
        {
            NTT_ENGINE_WARN("The system is not initialized");
            return;
        }

        ShutdownSystemImpl();

        THIS(initialized) = FALSE;
    }
} // namespace ntt::ecs