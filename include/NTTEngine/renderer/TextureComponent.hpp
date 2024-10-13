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
        Grid currentCell;
        Grid textureGrid;
        String tooltip;

        TextureComponent(resource_id_t id = RESOURCE_ID_DEFAULT,
                         u8 rowIndex = 0,
                         u8 colIndex = 0,
                         const String &tooltip = "")
            : id(id),
              currentCell(rowIndex, colIndex),
              tooltip(tooltip)
        {
        }

        String GetName() const override
        {
            return "TextureComponent";
        }
    };
} // namespace ntt::renderer
