#include "editor_tool.hpp"
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <array>

#include "imgui.h"

namespace ntt
{
    using namespace log;
    using namespace event;
    using namespace input;

    namespace
    {
        ToolType s_currentTool = MOVE;
        std::array<const char *, 3> s_toolNames = {"Move", "Scale", "Rotate"};

        b8 s_editorRun = FALSE;

        void OnEditorRun(event_code_t code, void *sender, const EventContext &context)
        {
            s_editorRun = TRUE;
        }

        void OnEditorStop(event_code_t code, void *sender, const EventContext &context)
        {
            s_editorRun = FALSE;
        }
    }

    void EditorToolInit()
    {
        PROFILE_FUNCTION();

        RegisterEvent(NTT_EDITOR_START, OnEditorRun);
        RegisterEvent(NTT_EDITOR_STOP, OnEditorStop);
    }

    void EditorToolUpdate()
    {
        PROFILE_FUNCTION();

        if (CHECK_PRESS(NTT_KEY_Q))
        {
            s_currentTool = MOVE;
            EventContext context;
            context.u8_data[0] = s_currentTool;
            TriggerEvent(NTT_EDITOR_TOOL_TYPE_CHANGED, nullptr, context);
        }

        if (CHECK_PRESS(NTT_KEY_W))
        {
            s_currentTool = SCALE;
            EventContext context;
            context.u8_data[0] = s_currentTool;
            TriggerEvent(NTT_EDITOR_TOOL_TYPE_CHANGED, nullptr, context);
        }

        if (CHECK_PRESS(NTT_KEY_E))
        {
            s_currentTool = ROTATE;
            EventContext context;
            context.u8_data[0] = s_currentTool;
            TriggerEvent(NTT_EDITOR_TOOL_TYPE_CHANGED, nullptr, context);
        }

        ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoTitleBar);

        for (i32 i = 0; i < s_toolNames.size(); i++)
        {
            if (ImGui::Selectable(s_toolNames[i], s_currentTool == i))
            {
                s_currentTool = static_cast<ToolType>(i);
                EventContext context;
                context.u8_data[0] = s_currentTool;
                TriggerEvent(NTT_EDITOR_TOOL_TYPE_CHANGED, nullptr, context);
            }
        }
        ImGui::End();
    }

    void EditorToolShutdown()
    {
        PROFILE_FUNCTION();
    }
} // namespace ntt
