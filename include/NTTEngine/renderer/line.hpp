#pragma once
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/structures/position.hpp>
#include <NTTEngine/structures/color.hpp>

namespace ntt
{
    struct Line : public ComponentBase
    {
        Position start;
        Position end;
        RGBAColor color;

        /**
         * 0 = solid
         * 1 = dashed
         * 2 = dotted
         */
        u8 lineType;

        Line(f32 startX = 0, f32 startY = 0,
             f32 endX = 100, f32 endY = 100,
             RGBAColor color = NTT_WHITE,
             u8 lineType = 0)
            : start(startX, startY),
              end(endX, endY),
              color(color), lineType(lineType)
        {
        }
    };
} // namespace ntt
