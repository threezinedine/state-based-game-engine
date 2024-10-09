#include "MoveXController.hpp"
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

    class MoveXController::Impl
    {
    public:
        entity_id_t entity;

        // TRUE if the button is hoverred and pressed
        // FALSE if the button is released
        b8 active = FALSE;

        f32 preMouseX = -1.0f;
    };

    MoveXController::MoveXController(void *data)
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
        m_impl->entity = *static_cast<entity_id_t *>(data);
    }

    MoveXController::~MoveXController()
    {
        PROFILE_FUNCTION();
    }

    void MoveXController::OnEnterImpl()
    {
        PROFILE_FUNCTION();
    }

    void MoveXController::OnExitImpl()
    {
        PROFILE_FUNCTION();
    }

    void MoveXController::OnUpdateImpl(f32 deltaTime)
    {
        PROFILE_FUNCTION();

        if (CheckState(NTT_BUTTON_LEFT, NTT_DOWN) && m_impl->active)
        {
            f32 mouseX = GetMousePosition().x;
            EventContext context;
            context.f32_data[0] = mouseX - m_impl->preMouseX;
            context.f32_data[1] = 0.0f;
            TriggerEvent(NTT_EDITOR_SELECTED_MOVE_REQUEST, this, context);

            m_impl->preMouseX = mouseX;
        }

        if (CheckState(NTT_BUTTON_LEFT, NTT_UP))
        {
            m_impl->active = FALSE;
            m_impl->preMouseX = -1.0f;
        }
    }

    void MoveXController::OnHover(HoveringContext &)
    {
        PROFILE_FUNCTION();

        if (CheckState(NTT_BUTTON_LEFT, NTT_PRESS))
        {
            m_impl->active = TRUE;
            m_impl->preMouseX = GetMousePosition().x;
        }
    }

    void MoveXController::OnHoverEnter()
    {
        PROFILE_FUNCTION();
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }

    void MoveXController::OnHoverExit()
    {
        PROFILE_FUNCTION();
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
} // namespace ntt
