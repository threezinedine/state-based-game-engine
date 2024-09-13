#include <NTTEngine/renderer/MouseHoveringSystem.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/core/logging.hpp>

namespace ntt::renderer
{
    using namespace log;

    namespace
    {
        b8 s_initialized = FALSE;

        // Store the pair texture vs callback for later update
        // Must be cleared after each update
        Dictionary<resource_id_t, HoveringCallback> s_callbacks;
    } // namespace

    void MouseHoveringSystemInit()
    {
        if (s_initialized)
        {
            NTT_ENGINE_WARN("The Mouse Hovering System is already initialized.");
            return;
        }

        s_initialized = TRUE;
    }

    void MouseHoveringSystemFunc(f32 delta, entity_id_t id)
    {
        auto callback = ECS_GET_COMPONENT(id, Hovering)->callback;
        auto texture = ECS_GET_COMPONENT(id, Texture);
        auto geo = ECS_GET_COMPONENT(id, Geometry);

        auto hovers = renderer::GetHoveredTexture();

        if (hovers.Contains(texture->id) && callback != nullptr)
        {
            s_callbacks[texture->id] = callback;
        }
    }

    void MouseHoveringSystemUpdate(f32 delta)
    {
        if (!s_initialized)
        {
            NTT_ENGINE_WARN("The Mouse Hovering System is not initialized.");
            return;
        }

        auto hovers = GetHoveredTexture();
        HoveringContext context;

        for (i32 i = hovers.size() - 1; i >= 0; i--)
        {
            if (!s_callbacks.Contains(hovers[i]))
            {
                continue;
            }
            s_callbacks[hovers[i]](context);

            if (context.goNext == FALSE)
            {
                break;
            }
        }

        s_callbacks.clear();
    }

    void MouseHoveringSystemShutdown()
    {
        if (!s_initialized)
        {
            NTT_ENGINE_WARN("The Mouse Hovering System is not initialized.");
            return;
        }

        s_initialized = FALSE;
    }
} // namespace ntt::renderer
