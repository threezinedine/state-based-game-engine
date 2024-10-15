#pragma once
#include <NTTEngine/resources/Resource.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    class StateResource : public Resource
    {
    public:
        StateResource(const ResourceInfo &info);
        ~StateResource();

    protected:
        resource_id_t LoadImpl() override;
        void UnloadImpl() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
