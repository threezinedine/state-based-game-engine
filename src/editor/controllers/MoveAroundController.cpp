#include "MoveAroundController.hpp"
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>

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
        b8 preState = FALSE;

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
    }

    void MoveAroundController::OnHover(HoveringContext &)
    {
        PROFILE_FUNCTION();

        if (CheckState(NTT_BUTTON_LEFT, NTT_DOWN))
        {
            if (m_impl->preState == FALSE)
            {
                TriggerEvent(NTT_EDITOR_SELECTED_MOVE_START, nullptr, {});
                m_impl->preState = TRUE;
            }

            // NTT_ENGINE_DEBUG("MoveAroundController: Left button pressed");
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
            context.f32_data[0] = mouse.x;
            context.f32_data[1] = mouse.y;
            TriggerEvent(NTT_EDITOR_SELECTED_MOVE_REQUEST, this, context);
        }
        else
        {
            if (m_impl->preState == TRUE)
            {
                TriggerEvent(NTT_EDITOR_SELECTED_MOVE_END, nullptr, {});
                m_impl->preState = FALSE;
            }
        }
    }

} // namespace ntt
