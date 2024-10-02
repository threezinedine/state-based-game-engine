#include <NTTEngine/application/script_system/ScriptResource.hpp>
#include <NTTEngine/application/script_system/script_system.hpp>

namespace ntt
{
#define THIS(exp) this->m_impl->exp
    using namespace script;

    class ScriptResource::Impl
    {
    public:
        const String m_name = "ScriptResource";
        String m_path;
        resource_id_t m_scriptId = INVALID_SCRIPT_ID;
    };

    ScriptResource::ScriptResource(const ResourceInfo &info)
        : m_impl(CreateScope<Impl>())
    {
        THIS(m_path) = info.path;
    }

    ScriptResource::~ScriptResource()
    {
    }

    const String &ScriptResource::GetName() const
    {
        return THIS(m_name);
    }

    resource_id_t ScriptResource::LoadImpl()
    {
        THIS(m_scriptId) = ScriptStoreLoad(THIS(m_path).RawString().c_str(), []() {});
        return THIS(m_scriptId);
    }

    void ScriptResource::UnloadImpl()
    {
        ScriptStoreUnload(THIS(m_scriptId));
    }
} // namespace ntt
