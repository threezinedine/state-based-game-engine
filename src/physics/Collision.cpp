#include <NTTEngine/physics/collision_system.hpp>
#include <NTTEngine/physics/collision.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <cmath>

namespace ntt
{
    String Collision::GetName() const
    {
        return "Collision";
    }

    JSON Collision::ToJSON() const
    {
        JSON data("{}");
        return data;
    }

    void Collision::FromJSON(const JSON &data)
    {
    }

    void Collision::OnEditorUpdate(std::function<void()> callback, void *data)
    {
    }
}