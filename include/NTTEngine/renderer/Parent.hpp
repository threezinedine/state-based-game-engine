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
    struct Parent : public ComponentBase
    {
        entity_id_t parentId;
        position_t posX;
        position_t posY;
        f32 angle;

        Parent(entity_id_t parentId = INVALID_ENTITY_ID,
               position_t posX = 0,
               position_t posY = 0,
               f32 angle = 0)
            : parentId(parentId), posX(posX),
              posY(posY), angle(angle)
        {
        }
    };
} // namespace ntt::renderer
