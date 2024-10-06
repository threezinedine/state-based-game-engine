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
    struct TextureComponent : public ComponentBase
    {
        resource_id_t id;
        u8 rowIndex;
        u8 colIndex;
        u8 priority;
        String tooltip;

        TextureComponent(resource_id_t id = RESOURCE_ID_DEFAULT, u8 rowIndex = 0,
                         u8 colIndex = 0, u8 priority = PRIORITY_0,
                         const String &tooltip = "")
            : id(id), rowIndex(rowIndex),
              colIndex(colIndex),
              priority(priority),
              tooltip(tooltip)
        {
        }
    };
} // namespace ntt::renderer
