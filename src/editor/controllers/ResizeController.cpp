#include "ResizeController.hpp"
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include "raylib.h"

namespace ntt
{
    using namespace input;

    class ResizeController::Impl
    {
    public:
        ResizeControllerData data;
        entity_id_t self;

        // TRUE if the button is hoverred and pressed
        b8 run = FALSE;

        Position preMousePos = {-1, -1};

        void OnSelectedResize(event_code_t code, void *sender, const EventContext &context)
        {
            f32 x = context.f32_data[0];
            f32 y = context.f32_data[1];

            auto selfParent = ECS_GET_COMPONENT(self, Parent);

            if (selfParent == nullptr)
            {
                NTT_ENGINE_WARN("The entity with ID {} does not have a parent component", self);
                return;
            }

            List<entity_id_t> selectedEntities = *reinterpret_cast<List<entity_id_t> *>(sender);
            if (!selectedEntities.Contains(data.entity))
            {
                return;
            }

            auto parentGeo = ECS_GET_COMPONENT(data.entity, Geometry);

            if (parentGeo == nullptr)
            {
                NTT_ENGINE_WARN("The entity with ID {} does not have a geometry component",
                                selfParent->parentId);
                return;
            }

            if (data.onResize == nullptr)
            {
                return;
            }

            data.onResize(x, y, parentGeo, selfParent);
        }
    };

    ResizeController::ResizeController(void *data)
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
        m_impl->data = *static_cast<ResizeControllerData *>(data);
    }

    ResizeController::~ResizeController()
    {
        PROFILE_FUNCTION();
    }

    void ResizeController::OnEnterImpl()
    {
        PROFILE_FUNCTION();
        Subscribe(NTT_EDITOR_SELECTED_RESIZE);
        m_impl->self = GetEntity();
    }

    void ResizeController::OnExitImpl()
    {
        PROFILE_FUNCTION();
    }

    void ResizeController::OnUpdateImpl(f32 deltaTime)
    {
        PROFILE_FUNCTION();

        if (m_impl->run && CheckState(NTT_BUTTON_LEFT, NTT_DOWN))
        {
            auto mouse = GetMousePosition();
            EventContext context;
            context.f32_data[0] = mouse.x - m_impl->preMousePos.x;
            context.f32_data[1] = mouse.y - m_impl->preMousePos.y;
            TriggerEvent(
                NTT_EDITOR_SELECTED_RESIZE_REQUEST,
                &m_impl->data,
                context);

            m_impl->preMousePos = {mouse.x, mouse.y};
        }

        if (CheckState(NTT_BUTTON_LEFT, NTT_UP))
        {
            m_impl->run = FALSE;
        }
    }

    void ResizeController::OnHoverEnter()
    {
        PROFILE_FUNCTION();

        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }

    void ResizeController::OnHover(HoveringContext &)
    {
        PROFILE_FUNCTION();

        if (CheckState(NTT_BUTTON_LEFT, NTT_PRESS))
        {
            m_impl->run = TRUE;
            m_impl->preMousePos = GetMousePosition();
        }
    }

    void ResizeController::OnHoverExit()
    {
        PROFILE_FUNCTION();
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    void ResizeController::OnEvent(event_code_t code, void *sender, const EventContext &context)
    {
        PROFILE_FUNCTION();

        switch (code)
        {
        case NTT_EDITOR_SELECTED_RESIZE:
            m_impl->OnSelectedResize(code, sender, context);
        }
    }
} // namespace ntt
