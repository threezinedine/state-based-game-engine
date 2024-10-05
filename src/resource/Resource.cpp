#include <NTTEngine/resources/Resource.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    using namespace log;
    using namespace event;

    class Resource::Impl
    {
    public:
        resource_id_t id;
        String name;
    };

    Resource::Resource(const ResourceInfo &info)
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();

        m_impl->name = info.name;
    }

    Resource::~Resource()
    {
        PROFILE_FUNCTION();
    }

    const String &Resource::GetName() const
    {
        PROFILE_FUNCTION();
        return m_impl->name;
    }

    resource_id_t Resource::Load()
    {
        PROFILE_FUNCTION();
        m_impl->id = LoadImpl();

        EventContext context;
        context.u32_data[0] = m_impl->id;
        TriggerEvent(NTT_RESOURCE_LOADED, nullptr, context);

        return m_impl->id;
    }

    void Resource::Unload()
    {
        PROFILE_FUNCTION();
        EventContext context;
        context.u32_data[0] = m_impl->id;
        TriggerEvent(NTT_RESOURCE_UNLOADED, nullptr, context);

        UnloadImpl();
    }
} // namespace ntt
