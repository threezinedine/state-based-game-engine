#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include "GraphicInterface.hpp"
#include <NTTEngine/structures/color.hpp>

namespace ntt::renderer
{
    using namespace ecs;

    /**
     * Store the information about the segment of the texture
     *      which is needed to draw on the screen.
     */
    struct Text : public ComponentBase
    {
        String text;
        u32 fontSize;
        RGBAColor color;

        Text(String text = "",
             u32 fontSize = 10,
             RGBAColor color = NTT_WHITE)
            : text(text),
              fontSize(fontSize),
              color(color)
        {
        }

        String GetName() const override
        {
            return "Text";
        }
    };
} // namespace ntt::renderer
