#include "MoveYController.hpp"
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>

#include "raylib.h"

namespace ntt
{
    using namespace log;
    using namespace ecs;
    using namespace input;

    class MoveYController::Impl
    {
    public:
        entity_id_t entity;

        // TRUE if the button is hoverred and pressed
        // FALSE if the button is released
        b8 active = FALSE;

        f32 preMouseY = -1.0f;
    };

    MoveYController::MoveYController(void *data)
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
        m_impl->entity = *static_cast<entity_id_t *>(data);
    }

    MoveYController::~MoveYController()
    {
        PROFILE_FUNCTION();
    }

    void MoveYController::OnEnterImpl()
    {
        PROFILE_FUNCTION();
    }

    void MoveYController::OnExitImpl()
    {
        PROFILE_FUNCTION();
    }

    void MoveYController::OnUpdateImpl(f32 deltaTime)
    {
        PROFILE_FUNCTION();

        if (CheckState(NTT_BUTTON_LEFT, NTT_DOWN) && m_impl->active)
        {
            f32 mouseY = GetMousePosition().y;
            EventContext context;
            context.f32_data[0] = 0.0f;
            context.f32_data[1] = mouseY - m_impl->preMouseY;
            TriggerEvent(NTT_EDITOR_SELECTED_MOVE_REQUEST, this, context);

            m_impl->preMouseY = mouseY;
        }

        if (CheckState(NTT_BUTTON_LEFT, NTT_UP))
        {
            m_impl->active = FALSE;
            m_impl->preMouseY = -1.0f;
        }
    }

    void MoveYController::OnHover(HoveringContext &)
    {
        PROFILE_FUNCTION();

        if (CheckState(NTT_BUTTON_LEFT, NTT_PRESS))
        {
            m_impl->active = TRUE;
            m_impl->preMouseY = GetMousePosition().y;
        }
    }

    void MoveYController::OnHoverEnter()
    {
        PROFILE_FUNCTION();
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }

    void MoveYController::OnHoverExit()
    {
        PROFILE_FUNCTION();
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
} // namespace ntt
