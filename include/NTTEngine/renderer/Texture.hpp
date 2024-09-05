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
    struct Texture : public ComponentBase
    {
        texture_id_t id;
        u8 rowIndex;
        u8 colIndex;

        Texture(texture_id_t id = DEFAULT_TEXTURE, u8 rowIndex = 0, u8 colIndex = 0)
            : id(id), rowIndex(rowIndex), colIndex(colIndex)
        {
        }
    };
} // namespace ntt::renderer
