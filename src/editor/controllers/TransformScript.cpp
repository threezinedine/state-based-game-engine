#include "TransformScript.hpp"
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include "raylib.h"

namespace ntt
{
    using namespace input;

    class TransformScript::Impl
    {
    public:
        TransformScriptData data;
        entity_id_t self;

        // TRUE if the button is hoverred and pressed
        b8 run = FALSE;

        Position preMousePos = {-1, -1};
        Position startPos = {-1, -1};

        void OnSelectedResize(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            if (data.onResizeMain == nullptr)
            {
                return;
            }

            if (data.onAddEntReset == nullptr)
            {
                return;
            }

            data.onAddEntReset(self);
        }
    };

    TransformScript::TransformScript(void *data)
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
        m_impl->data = *static_cast<TransformScriptData *>(data);
    }

    TransformScript::~TransformScript()
    {
        PROFILE_FUNCTION();
    }

    void TransformScript::OnEnterImpl()
    {
        PROFILE_FUNCTION();
        m_impl->self = GetEntity();
        Subscribe(NTT_EDITOR_TRANSFORM_CHANGED);
    }

    void TransformScript::OnExitImpl()
    {
        PROFILE_FUNCTION();
    }

    void TransformScript::OnUpdateImpl(f32 deltaTime)
    {
        PROFILE_FUNCTION();

        if (m_impl->run && CheckState(NTT_BUTTON_LEFT, NTT_DOWN))
        {
            auto mouse = GetMousePosition();
            EventContext context;
            context.f32_data[0] = mouse.x - m_impl->preMousePos.x;
            context.f32_data[1] = mouse.y - m_impl->preMousePos.y;
            context.f32_data[2] = mouse.x;
            context.f32_data[3] = mouse.y;

            TriggerEvent(
                NTT_EDITOR_TRANSFORM_CHANGED_REQUEST,
                &m_impl->data,
                context);

            m_impl->preMousePos = {mouse.x, mouse.y};
        }

        if (CheckState(NTT_BUTTON_LEFT, NTT_UP) && m_impl->run)
        {
            m_impl->run = FALSE;
            m_impl->preMousePos = {-1, -1};

            if (m_impl->data.onEntityChanged != nullptr)
            {
                m_impl->data.onEntityChanged(
                    m_impl->startPos,
                    GetMousePosition());
            }

            m_impl->startPos = {-1, -1};
        }
    }

    void TransformScript::OnHoverEnter()
    {
        PROFILE_FUNCTION();

        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }

    void TransformScript::OnHover(HoveringContext &)
    {
        PROFILE_FUNCTION();

        if (CheckState(NTT_BUTTON_LEFT, NTT_PRESS))
        {
            m_impl->run = TRUE;
            m_impl->preMousePos = GetMousePosition();
            m_impl->startPos = GetMousePosition();
        }
    }

    void TransformScript::OnHoverExit()
    {
        PROFILE_FUNCTION();
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    void TransformScript::OnEvent(event_code_t code, void *sender, const EventContext &context)
    {
        PROFILE_FUNCTION();

        switch (code)
        {
        case NTT_EDITOR_TRANSFORM_CHANGED:
            m_impl->OnSelectedResize(code, sender, context);
        }
    }
} // namespace ntt
