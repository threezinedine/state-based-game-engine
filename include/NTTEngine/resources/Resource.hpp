#pragma once
#include "resource_common.h"

namespace ntt
{
    /**
     * Interfaces of all resources, all other resources must
     *      inherit from this interface.
     */
    class Resource
    {
    public:
        virtual ~Resource() = default;

        virtual const String &GetName() const = 0;

        /**
         * Actually load the resource into the memory
         *      rather just loading the the resource's metadata.
         *
         * @return True if the resource is loaded successfully, otherwise false.
         */
        virtual resource_id_t Load() = 0;

        /**
         * Unload the resource from the memory.
         */
        virtual void Unload() = 0;
    };
} // namespace ntt
