#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/position.hpp>
#include <NTTEngine/ecs/ecs.hpp>

namespace ntt::physics
{
    /**
     * The velocity of an entity.
     */
    struct Mass : public ecs::ComponentBase
    {
        position_t velocity_x;
        position_t velocity_y;

        position_t acc_x;
        position_t acc_y;

        position_t force_x;
        position_t force_y;

        f32 mass;

        Mass(f32 mass)
            : mass(mass), velocity_x(0),
              velocity_y(0), acc_x(0),
              acc_y(0)
        {
        }

        void AddForceConst(position_t x, position_t y)
        {
            force_x = x;
            force_y = y;
            acc_x = force_x / mass;
            acc_y = force_y / mass;
        }
    };
} // namespace ntt
