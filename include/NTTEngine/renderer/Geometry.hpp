#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/position.hpp>
#include <NTTEngine/structures/size.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include "GraphicInterface.hpp"

namespace ntt::renderer
{
    using namespace ecs;

    /**
     * Component which store the rendering information
     *      of position, size of the entity, etc.
     */
    struct Geometry : public ComponentBase
    {
        position_t x;      ///< The x position in pixel
        position_t y;      ///< The y position in pixel
        ntt_size_t width;  ///< The width in pixel
        ntt_size_t height; ///< The height in pixel
        f32 rotation;
        u8 priority;
        RGBAColor color;

        Geometry(position_t x = POSITION_DEFAULT, position_t y = POSITION_DEFAULT,
                 ntt_size_t width = SIZE_DEFAULT, ntt_size_t height = SIZE_DEFAULT,
                 f32 rotation = 0.0f, u8 priority = PRIORITY_0, RGBAColor color = NTT_WHITE)
            : x(x), y(y), width(width), height(height), rotation(rotation),
              priority(priority), color(color)
        {
        }
    };
} // namespace ntt::renderer
