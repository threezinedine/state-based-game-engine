#pragma once
#include <NTTEngine/resources/resources.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt::audio
{
    class AudioResource : public Resource
    {
    public:
        AudioResource(const ResourceInfo &info);
        ~AudioResource();

    protected:
        resource_id_t LoadImpl() override;
        void UnloadImpl() override;

    private:
        class Impl;
        Scope<Impl> m_Impl;
    };
} // namespace ntt::audio
