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

        /**
         * Retrieve the configuration information of the resource.
         */
        ResourceInfo *GetInfo();

        /**
         * Check if the resource is loaded or not.
         *
         * @return TRUE if the resource is loaded, otherwise FALSE.
         *      If TRUE, then the resource cannot be loaded again
         *          or the warning will be logged.
         */
        b8 IsLoaded() const;

        /**
         * Actually load the resource into the memory
         *      rather just loading the the resource's metadata.
         * If the resource is already loaded, then nothing will be loaded
         *      and the warning will be logged.
         *
         * @return True if the resource is loaded successfully, otherwise false.
         */
        resource_id_t Load();

        /**
         * Unload the resource from the memory. If the resource is not loaded,
         *      then nothing will be unloaded and the warning will be logged.
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
