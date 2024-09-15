#include <NTTEngine/renderer/MouseHoveringSystem.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/renderer/Texture.hpp>

namespace ntt::renderer
{
    using namespace log;

    namespace
    {
        Dictionary<resource_id_t, HoveringCallback> s_callbacks;
    } // namespace

#define THIS(exp) (m_impl->exp)

    class MouseHoveringSystem::Impl
    {
    public:
    };

    MouseHoveringSystem::MouseHoveringSystem()
        : System()
    {
        m_impl = CreateScope<Impl>();
    }

    MouseHoveringSystem::~MouseHoveringSystem()
    {
    }

    void MouseHoveringSystem::InitSystemImpl()
    {
    }

    void MouseHoveringSystem::InitEntityImpl(entity_id_t id)
    {
    }

    void MouseHoveringSystem::UpdateImpl(f32 delta, entity_id_t id)
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

    void MouseHoveringSystem::ShutdownEntityImpl(entity_id_t id)
    {
    }

    void MouseHoveringSystem::ShutdownSystemImpl()
    {
    }

    void MouseHoveringSystemUpdate(f32 delta)
    {
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

} // namespace ntt::renderer
