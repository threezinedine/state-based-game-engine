#pragma once
#include <NTTEngine/resources/Resource.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace memory;

    class ScriptResource : public Resource
    {
    public:
        ScriptResource(const String &name, const String &path);
        ~ScriptResource();

        const String &GetName() const override;

    protected:
        resource_id_t LoadImpl() override;
        void UnloadImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
