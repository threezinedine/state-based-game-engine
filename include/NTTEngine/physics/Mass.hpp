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

        Mass(f32 mass = 1.0f, position_t velocity_x = 0,
             position_t velocity_y = 0, position_t acc_x = 0,
             position_t acc_y = 0)
            : mass(mass), velocity_x(velocity_x),
              velocity_y(velocity_y), acc_x(acc_x),
              acc_y(acc_y),
              force_x(0), force_y(0)
        {
        }

        void AddForceConst(position_t x, position_t y);

        String GetName() const override;
        void FromJSON(const JSON &json) override;
        JSON ToJSON() const override;

        void OnEditorUpdate(std::function<void()> onChanged = nullptr, void *data = nullptr) override;
    };
} // namespace ntt
