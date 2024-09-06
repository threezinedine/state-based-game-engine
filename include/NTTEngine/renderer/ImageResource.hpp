#pragma once
#include <NTTEngine/resources/Resource.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt::renderer
{
    class ImageResource : public Resource
    {
    public:
        ImageResource(const ResourceInfo &info);
        ~ImageResource() override;

        const String &GetName() const override;

        resource_id_t Load() override;
        void Unload() override;

    private:
        class Impl;
        Scope<Impl> m_Impl;
    };
} // namespace ntt::renderer
