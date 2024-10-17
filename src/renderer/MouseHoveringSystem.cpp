#include <NTTEngine/renderer/MouseHoveringSystem.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/renderer/TextureComponent.hpp>
#include <NTTEngine/core/profiling.hpp>
#include "raylib.h"

namespace ntt
{
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
        auto texture = ECS_GET_COMPONENT(id, TextureComponent);

        auto callback = hovering->callback;
        auto onEnterCallback = hovering->onEnterCallback;
        auto onExitCallback = hovering->onExitCallback;

        auto hovers = GetHoveredTexture();

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

        if (onEnterCallback != nullptr)
        {
            auto context = HoveringContext();
            onEnterCallback(context);
        }

        if (hovering->cursor)
        {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        }

        if (hovering->hoveredCell.col != 255 &&
            hovering->hoveredCell.row != 255 &&
            texture != nullptr)
        {
            hovering->prevHoveredCell = texture->currentCell;
            texture->currentCell = hovering->hoveredCell;
        }
    }

    void MouseHoveringSystem::ShutdownEntity(entity_id_t id)
    {
        PROFILE_FUNCTION();
        s_callbacks.erase(id);
        s_prevHovered.RemoveItem(id);
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

            try
            {
                s_callbacks[hovers[i]](context);
            }
            catch (const std::exception &e)
            {
                NTT_ENGINE_ERROR("Error: {}", e.what());
            }

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

            if (hovering->onExitCallback != nullptr)
            {
                HoveringContext context;
                hovering->onExitCallback(context);
            }

            if (hovering->cursor)
            {
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }

            auto texture = ECS_GET_COMPONENT(entity, TextureComponent);

            if (hovering->prevHoveredCell.col != 255 &&
                hovering->prevHoveredCell.row != 255 &&
                texture != nullptr)
            {
                texture->currentCell = hovering->prevHoveredCell;
            }
        }

        s_callbacks.clear();
    }

} // namespace ntt
