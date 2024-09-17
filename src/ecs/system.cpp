#include <NTTEngine/ecs/ecs.hpp>

#define THIS(exp) (m_impl->exp)

namespace ntt::ecs
{
    class System::Impl
    {
    public:
        b8 initialized = FALSE;
    };

    System::System()
    {
        m_impl = CreateScope<Impl>();
    }

    System::~System()
    {
    }

    void System::InitSystem()
    {
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
        if (THIS(initialized) == FALSE)
        {
            NTT_ENGINE_WARN("The system is not initialized");
            return;
        }

        InitEntityImpl(id);
    }

    b8 System::ShouldUpdate(entity_id_t id)
    {
        return ECSGetEntity(id)->active;
    }

    void System::Update(f32 delta, entity_id_t id)
    {
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
        if (THIS(initialized) == FALSE)
        {
            NTT_ENGINE_WARN("The system is not initialized");
            return;
        }

        ShutdownEntityImpl(id);
    }

    void System::ShutdownSystem()
    {
        if (THIS(initialized) == FALSE)
        {
            NTT_ENGINE_WARN("The system is not initialized");
            return;
        }

        ShutdownSystemImpl();

        THIS(initialized) = FALSE;
    }
} // namespace ntt::ecs