#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt
{
    /**
     * If need to change the type of the position, then
     *      does not need to change any code in the engine
     */
    using position_t = f32;

    /**
     * This is used for representing a position
     *      (in this engine its unit is pixel)
     */
    struct Position
    {
        position_t x; ///< The x position in pixel
        position_t y; ///< The y position in pixel

        Position() : x(0), y(0) {}
        Position(position_t x, position_t y) : x(x), y(y) {}
    };
} // namespace ntt
