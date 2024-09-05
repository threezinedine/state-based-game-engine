#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>

using namespace ntt;
using namespace ntt::ecs;

namespace ntt::physics
{
    void MassFunc(f32 delta, entity_id_t id, List<entity_id_t> others);
} // namespace ntt
