#pragma once
#include "resource_common.h"
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/object.hpp>

namespace ntt
{
    using namespace memory;

    /**
     * Interfaces of all resources, all other resources must
     *      inherit from this interface.
     */
    class Resource : public Object
    {
    public:
        Resource(const ResourceInfo &info);
        virtual ~Resource();

        ResourceInfo *GetInfo();

        /**
         * Actually load the resource into the memory
         *      rather just loading the the resource's metadata.
         *
         * @return True if the resource is loaded successfully, otherwise false.
         */
        resource_id_t Load();

        /**
         * Unload the resource from the memory.
         */
        void Unload();

    protected:
        virtual resource_id_t LoadImpl() = 0;
        virtual void UnloadImpl() = 0;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
