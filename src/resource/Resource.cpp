#include <NTTEngine/resources/Resource.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/logging.hpp>

namespace ntt
{
    using namespace log;
    using namespace event;

    class Resource::Impl
    {
    public:
        resource_id_t id;
    };

    Resource::Resource()
    {
        m_impl = CreateScope<Impl>();
    }

    Resource::~Resource()
    {
    }

    resource_id_t Resource::Load()
    {
        m_impl->id = LoadImpl();

        EventContext context;
        context.u32_data[0] = m_impl->id;
        TriggerEvent(NTT_RESOURCE_LOADED, nullptr, context);

        return m_impl->id;
    }

    void Resource::Unload()
    {
        EventContext context;
        context.u32_data[0] = m_impl->id;
        TriggerEvent(NTT_RESOURCE_UNLOADED, nullptr, context);

        UnloadImpl();
    }
} // namespace ntt
