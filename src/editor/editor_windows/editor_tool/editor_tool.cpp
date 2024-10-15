#include "editor_tool.hpp"
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <array>
#include <NTTEngine/core/profiling.hpp>

#include "imgui.h"

namespace ntt
{
    using namespace log;
    using namespace event;
    using namespace input;

    class EditorTool::Impl
    {
    public:
        ToolType currentTool = MOVE;
        std::array<const char *, 3> toolNames = {"Move", "Scale", "Rotate"};

        b8 editorRun = FALSE;

        void OnEditorRun(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            editorRun = TRUE;
        }

        void OnEditorStop(event_code_t code, void *sender, const EventContext &context)
        {
            PROFILE_FUNCTION();
            editorRun = FALSE;
        }
    };

    EditorTool::EditorTool()
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();
    }

    EditorTool::~EditorTool()
    {
        PROFILE_FUNCTION();
    }

    void EditorTool::InitImpl()
    {
        PROFILE_FUNCTION();

        // RegisterEvent(NTT_EDITOR_START, OnEditorRun);
        // RegisterEvent(NTT_EDITOR_STOP, OnEditorStop);
    }

    void EditorTool::UpdateImpl(b8 *p_open, ImGuiWindowFlags flags)
    {
        PROFILE_FUNCTION();

        if (CHECK_PRESS(NTT_KEY_Q))
        {
            m_impl->currentTool = MOVE;
            EventContext context;
            context.u8_data[0] = m_impl->currentTool;
            TriggerEvent(NTT_EDITOR_TOOL_TYPE_CHANGED, nullptr, context);
        }

        if (CHECK_PRESS(NTT_KEY_W))
        {
            m_impl->currentTool = SCALE;
            EventContext context;
            context.u8_data[0] = m_impl->currentTool;
            TriggerEvent(NTT_EDITOR_TOOL_TYPE_CHANGED, nullptr, context);
        }

        if (CHECK_PRESS(NTT_KEY_E))
        {
            m_impl->currentTool = ROTATE;
            EventContext context;
            context.u8_data[0] = m_impl->currentTool;
            TriggerEvent(NTT_EDITOR_TOOL_TYPE_CHANGED, nullptr, context);
        }

        ImGui::Begin("Tools", p_open, ImGuiWindowFlags_NoTitleBar);

        for (i32 i = 0; i < m_impl->toolNames.size(); i++)
        {
            if (ImGui::Selectable(m_impl->toolNames[i], m_impl->currentTool == i))
            {
                m_impl->currentTool = static_cast<ToolType>(i);
                EventContext context;
                context.u8_data[0] = m_impl->currentTool;
                TriggerEvent(NTT_EDITOR_TOOL_TYPE_CHANGED, nullptr, context);
            }
        }
        ImGui::End();
    }

    void EditorTool::ShutdownImpl()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt
