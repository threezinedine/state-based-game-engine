#include "MoveAroundController.hpp"
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>

// TODO: should be wrapped later
#include "raylib.h"

namespace ntt
{
    using namespace log;
    using namespace ecs;
    using namespace input;

    class MoveAroundController::Impl
    {
    public:
        entity_id_t entity;

        // True if the left button is pressed
        b8 active = FALSE;
        Position preMousePos = {-1, -1};

        void OnSelectedMove(event_code_t code, void *sender, const EventContext &context)
        {
            f32 x = context.f32_data[0];
            f32 y = context.f32_data[1];

            if (!reinterpret_cast<List<entity_id_t> *>(sender)->Contains(entity))
            {
                return;
            }

            auto entGeo = ECS_GET_COMPONENT(entity, Geometry);
            if (entGeo == nullptr)
            {
                NTT_ENGINE_WARN("The entity with ID {} does not have a geometry component",
                                entity);
                return;
            }

            auto entParent = ECS_GET_COMPONENT(entity, Parent);

            if (entParent == nullptr)
            {
                entGeo->x += x;
                entGeo->y += y;
            }
            else
            {
                entParent->posX += x;
                entParent->posY += y;
            }
        }
    };

    MoveAroundController::MoveAroundController(void *data)
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
        m_impl->entity = *static_cast<entity_id_t *>(data);
    }

    MoveAroundController::~MoveAroundController()
    {
        PROFILE_FUNCTION();
    }

    void MoveAroundController::OnEnterImpl()
    {
        PROFILE_FUNCTION();

        Subscribe(NTT_EDITOR_SELECTED_MOVE);
    }

    void MoveAroundController::OnEvent(event_code_t code, void *sender, const EventContext &context)
    {
        PROFILE_FUNCTION();

        switch (code)
        {
        case NTT_EDITOR_SELECTED_MOVE:
            m_impl->OnSelectedMove(code, sender, context);
            break;
        }
    }

    void MoveAroundController::OnExitImpl()
    {
        PROFILE_FUNCTION();
    }

    void MoveAroundController::OnUpdateImpl(f32 deltaTime)
    {
        PROFILE_FUNCTION();

        if (CheckState(NTT_BUTTON_LEFT, NTT_DOWN) && m_impl->active)
        {
            auto geo = GetComponent<Geometry>();
            auto entGeo = ECS_GET_COMPONENT(m_impl->entity, Geometry);

            if (entGeo == nullptr)
            {
                NTT_ENGINE_WARN("The entity with ID {} does not have a geometry component",
                                m_impl->entity);
                return;
            }

            if (geo == nullptr)
            {
                NTT_ENGINE_WARN("The entity with ID {} does not have a geometry component",
                                GetEntity());
                return;
            }

            EventContext context;
            auto mouse = GetMousePosition();
            context.f32_data[0] = mouse.x - m_impl->preMousePos.x;
            context.f32_data[1] = mouse.y - m_impl->preMousePos.y;
            TriggerEvent(NTT_EDITOR_SELECTED_MOVE_REQUEST, this, context);

            m_impl->preMousePos = {mouse.x, mouse.y};
        }

        if (CheckState(NTT_BUTTON_LEFT, NTT_UP))
        {
            m_impl->active = FALSE;
            m_impl->preMousePos = {-1, -1};
        }
    }

    void MoveAroundController::OnHoverEnter()
    {
        PROFILE_FUNCTION();

        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }

    void MoveAroundController::OnHover(HoveringContext &)
    {
        PROFILE_FUNCTION();

        if (CheckState(NTT_BUTTON_LEFT, NTT_PRESS))
        {
            m_impl->preMousePos = {GetMousePosition().x, GetMousePosition().y};
            m_impl->active = TRUE;
        }
    }

    void MoveAroundController::OnHoverExit()
    {
        PROFILE_FUNCTION();

        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

} // namespace ntt
