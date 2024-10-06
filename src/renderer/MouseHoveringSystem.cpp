#include <NTTEngine/renderer/MouseHoveringSystem.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/renderer/TextureComponent.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt::renderer
{
    using namespace log;

    namespace
    {
        Dictionary<entity_id_t, HoveringCallback> s_callbacks;

        List<entity_id_t> s_prevHovered;
    } // namespace

#define THIS(exp) (m_impl->exp)

    class MouseHoveringSystem::Impl
    {
    public:
    };

    MouseHoveringSystem::MouseHoveringSystem()
        : System()
    {
        PROFILE_FUNCTION();
        m_impl = CreateScope<Impl>();
    }

    MouseHoveringSystem::~MouseHoveringSystem()
    {
        PROFILE_FUNCTION();
    }

    void MouseHoveringSystem::InitSystem()
    {
        PROFILE_FUNCTION();
        s_prevHovered.clear();
    }

    void MouseHoveringSystem::InitEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
    }

    void MouseHoveringSystem::Update(f32 delta, entity_id_t id)
    {
        PROFILE_FUNCTION();
        auto hovering = ECS_GET_COMPONENT(id, Hovering);
        auto geo = ECS_GET_COMPONENT(id, Geometry);

        auto callback = hovering->callback;
        auto onEnterCallback = hovering->onEnterCallback;
        auto onExitCallback = hovering->onExitCallback;

        auto hovers = renderer::GetHoveredTexture();

        if (!hovers.Contains(id))
        {
            return;
        }

        if (callback != nullptr)
        {
            s_callbacks[id] = callback;
        }

        if (s_prevHovered.Contains(id))
        {
            return;
        }

        s_prevHovered.push_back(id);

        if (onEnterCallback == nullptr)
        {
            return;
        }
        auto context = HoveringContext();
        onEnterCallback(context);
    }

    void MouseHoveringSystem::ShutdownEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
    }

    void MouseHoveringSystem::ShutdownSystem()
    {
        PROFILE_FUNCTION();
    }

    void MouseHoveringSystemUpdate(f32 delta)
    {
        PROFILE_FUNCTION();
        const List<entity_id_t> &hovers = GetHoveredTexture();
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

        for (auto entity : s_prevHovered)
        {
            auto hovering = ECS_GET_COMPONENT(entity, Hovering);

            if (hovers.Contains(entity))
            {
                continue;
            }

            s_prevHovered.RemoveItem(entity);

            if (hovering->onExitCallback == nullptr)
            {
                continue;
            }

            HoveringContext context;
            hovering->onExitCallback(context);
        }

        s_callbacks.clear();
    }

} // namespace ntt::renderer
