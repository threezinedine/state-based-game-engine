#pragma once
#include <NTTEngine/resources/Resource.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    class ScriptResource : public Resource
    {
    public:
        ScriptResource(const ResourceInfo &info);
        ~ScriptResource();

    protected:
        resource_id_t LoadImpl() override;
        void UnloadImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
