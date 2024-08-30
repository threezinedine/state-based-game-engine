#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt::resources
{
    /**
     * Interfaces of all resources, all other resources must
     *      inherit from this interface.
     */
    class Resource
    {
    public:
        virtual ~Resource() = default;

        /**
         * Actually load the resource into the memory
         *      rather just loading the the resource's metadata.
         *
         * @return True if the resource is loaded successfully, otherwise false.
         */
        virtual b8 Load() = 0;

        /**
         * Unload the resource from the memory.
         */
        virtual void Unload() = 0;
    };
} // namespace ntt::resources
