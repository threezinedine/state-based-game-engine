#pragma once

#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/parser/json.hpp>

namespace ntt
{
    /**
     * If need to change the type of the position, then
     *      does not need to change any code in the engine
     */
    using ntt_size_t = f32;
    constexpr ntt_size_t SIZE_DEFAULT = f32{0} - 1;

    /**
     * This is used for representing asize
     *      (in this engine its unit is pixel)
     *
     * The SIZE_DEFAULT value for the size is ntt_size_t{-1}
     */
    struct Size
    {
        ntt_size_t width;  ///< The width in pixel
        ntt_size_t height; ///< The height in pixel

        Size() : width(SIZE_DEFAULT), height(SIZE_DEFAULT) {}
        Size(ntt_size_t width) : width(width), height(SIZE_DEFAULT) {}
        Size(ntt_size_t width, ntt_size_t height) : width(width), height(height) {}

        b8 operator==(const Size &other) const
        {
            return width == other.width && height == other.height;
        }

        void FromJSON(const JSON &json);
        JSON ToJSON() const;
        void OnEditorUpdate(
            std::function<void()> onChanged = nullptr,
            f32 ratio = -1.0f);
    };

    typedef Size (*SizeTransform)(const Size &size);
} // namespace ntt
