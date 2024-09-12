#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt::renderer
{
    using namespace ecs;

    void RenderInitFunc(entity_id_t id);
    void RenderFunc(f32 delta, entity_id_t id);
    void RenderShutdownFunc(entity_id_t id);

    void SpriteRenderFunc(f32 delta, entity_id_t id);
} // namespace ntt::renderer
