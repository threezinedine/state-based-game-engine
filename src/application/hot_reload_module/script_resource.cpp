#include <NTTEngine/application/hot_reload_module/ScriptResource.hpp>

namespace ntt
{
#define THIS(exp) this->m_impl->exp

    class ScriptResource::Impl
    {
    public:
    };

    ScriptResource::ScriptResource(const String &name, const String &path)
        : m_impl(CreateScope<Impl>())
    {
    }

    ScriptResource::~ScriptResource()
    {
    }

    const String &ScriptResource::GetName() const
    {
        return "ScriptResource";
    }

    resource_id_t ScriptResource::LoadImpl()
    {
        return RESOURCE_ID_DEFAULT;
    }

    void ScriptResource::UnloadImpl()
    {
    }
} // namespace ntt
