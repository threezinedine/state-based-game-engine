#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt
{
    /**
     * This is used for representing a position
     *      (in this engine its unit is pixel)
     */
    struct Position
    {
        i32 pix_x; ///< The x position in pixel
        i32 pix_y; ///< The y position in pixel
    };
} // namespace ntt
