#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>

#define COLLISION_NAME "Collision"

namespace ntt
{
    using CollisionCallback = std::function<void(List<entity_id_t>)>;

    /**
     * Collision component with no data (at least for now).
     */
    struct Collision : public ComponentBase
    {
        CollisionCallback callback = nullptr;

        Collision() = default;

        String GetName() const override;

        JSON ToJSON() const override;
        void FromJSON(const JSON &data) override;

        void OnEditorUpdate(std::function<void()> callback = nullptr, void *data = nullptr) override;
    };
} // namespace ntt
