#pragma once
#include <NTTEngine/core/memory.hpp>
#include "Resource.hpp"

/**
 * Managing all resources (supports multiple resource types)
 *      - Texture
 *      - Audio
 *      - Font
 *      - etc.
 *
 * Other modules who need to use any external resources must
 *      access through this module.
 */
namespace ntt::resource
{
    /**
     * All supported resource types.
     */
    enum ResourceType
    {
        IMAGE = 0, ///< Image resource type (.png only)
        AUDIO = 1, ///< Audio resource type (.wav only)
        FONT = 2,  ///< Font resource type (.ttf only)
    };
} // namespace ntt::resource
