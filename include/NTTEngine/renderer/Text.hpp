#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include "GraphicInterface.hpp"

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
        u8 priority;

        Text(String text = "", u32 fontSize = 10, u8 priority = 0)
            : text(text),
              fontSize(fontSize),
              priority(priority)
        {
        }
    };
} // namespace ntt::renderer
