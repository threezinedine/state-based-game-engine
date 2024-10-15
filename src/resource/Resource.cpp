#include <NTTEngine/resources/Resource.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt
{
    class Resource::Impl
    {
    public:
        resource_id_t id;
        b8 loaded;
        ResourceInfo info;
    };

    Resource::Resource(const ResourceInfo &info)
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();

        m_impl->info = info;
        m_impl->loaded = FALSE;
    }

    Resource::~Resource()
    {
        PROFILE_FUNCTION();
    }

    ResourceInfo *Resource::GetInfo()
    {
        PROFILE_FUNCTION();
        return &(m_impl->info);
    }

    b8 Resource::IsLoaded() const
    {
        PROFILE_FUNCTION();
        return m_impl->loaded;
    }

    resource_id_t Resource::Load()
    {
        PROFILE_FUNCTION();

        if (m_impl->loaded)
        {
            NTT_ENGINE_WARN("The resource {} is already loaded.", m_impl->info.name);
            return m_impl->id;
        }

        m_impl->id = LoadImpl();

        EventContext context;
        context.u32_data[0] = m_impl->id;
        TriggerEvent(NTT_RESOURCE_LOADED, nullptr, context);

        m_impl->loaded = TRUE;

        return m_impl->id;
    }

    void Resource::Unload()
    {
        PROFILE_FUNCTION();

        if (!m_impl->loaded)
        {
            NTT_ENGINE_WARN("The resource {} is not loaded.", m_impl->info.name);
            return;
        }

        EventContext context;
        context.u32_data[0] = m_impl->id;
        TriggerEvent(NTT_RESOURCE_UNLOADED, nullptr, context);

        m_impl->loaded = FALSE;

        UnloadImpl();
    }
} // namespace ntt
