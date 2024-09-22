#pragma once

#include <NTTEngine/defines.hpp>

namespace ntt
{
    /**
     * If need to change the type of the position, then
     *      does not need to change any code in the engine
     */
    using size_t = f32;
    constexpr size_t SIZE_DEFAULT = f32{0} - 1;

    /**
     * This is used for representing asize
     *      (in this engine its unit is pixel)
     *
     * The SIZE_DEFAULT value for the size is size_t{-1}
     */
    struct Size
    {
        size_t width;  ///< The width in pixel
        size_t height; ///< The height in pixel

        Size() : width(SIZE_DEFAULT), height(SIZE_DEFAULT) {}
        Size(size_t width) : width(width), height(SIZE_DEFAULT) {}
        Size(size_t width, size_t height) : width(width), height(height) {}

        b8 operator==(const Size &other) const
        {
            return width == other.width && height == other.height;
        }
    };
} // namespace ntt
