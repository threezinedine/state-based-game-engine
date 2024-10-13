#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include "GraphicInterface.hpp"
#include <NTTEngine/core/time.hpp>

namespace ntt::renderer
{
    using namespace ecs;

    /**
     * Store the information about the segment of the texture
     *      which is needed to draw on the screen.
     */
    struct Sprite : public ComponentBase
    {
        f32 changePerMilis;
        List<std::pair<u8, u8>> cells;
        u32 currentCell;
        Timer timer;

        Sprite(List<std::pair<u8, u8>> cells = {},
               f32 changePerMilis = 1000)
            : cells(cells), changePerMilis(changePerMilis),
              currentCell(0)
        {
            timer.Reset();
        }

        String GetName() const override
        {
            return "Sprite";
        }
    };
} // namespace ntt::renderer
