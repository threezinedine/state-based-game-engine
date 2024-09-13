#include <NTTEngine/renderer/MouseHoveringSystem.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>

namespace ntt::renderer
{
    void MouseHoveringSystemFunc(f32 delta, entity_id_t id)
    {
        auto callback = ECS_GET_COMPONENT(id, HoveringComponent)->callback;
        auto texture = ECS_GET_COMPONENT(id, Texture);
        auto geo = ECS_GET_COMPONENT(id, Geometry);

        auto hovers = renderer::GetHoveredTexture();

        if (hovers.Contains(texture->id))
        {
            if (callback)
            {
                callback();
            }
        }
    }
} // namespace ntt::renderer
