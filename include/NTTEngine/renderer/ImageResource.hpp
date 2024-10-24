#pragma once
#include <NTTEngine/resources/Resource.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    class ImageResource : public Resource
    {
    public:
        ImageResource(const ResourceInfo &info);
        ~ImageResource() override;

    protected:
        resource_id_t LoadImpl() override;
        void UnloadImpl() override;

    private:
        class Impl;
        Scope<Impl> m_Impl;
    };
} // namespace ntt
