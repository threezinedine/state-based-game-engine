#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt::renderer
{
    using namespace ecs;

    void RenderFunc(f32 delta, entity_id_t id, List<entity_id_t> others);
} // namespace ntt::renderer
