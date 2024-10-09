#include "editor_entity_window.hpp"
#include "imgui.h"
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

namespace ntt
{
    using namespace ecs;
    using namespace event;

    namespace
    {
        List<entity_id_t> s_selectedEntities;

        void OnEntitySelected(event_code_t code, void *sender, const EventContext &context)
        {
            s_selectedEntities.clear();
            s_selectedEntities.push_back(context.u32_data[0]);
        }

        void OnEntityAppend(event_code_t code, void *sender, const EventContext &context)
        {
            s_selectedEntities.push_back(context.u32_data[0]);
        }
    }

    void EditorEntityWindowInit()
    {
        s_selectedEntities.clear();

        RegisterEvent(NTT_EDITOR_CHOOSE_ENTITY, OnEntitySelected);
        RegisterEvent(NTT_EDITOR_APPEND_ENTITY, OnEntityAppend);
    }

    void EditorEntityWindowUpdate(b8 *p_open, b8 isRunning)
    {
        if (ImGui::Begin("Entity", p_open, isRunning ? ImGuiWindowFlags_NoInputs : 0))
        {
            if (s_selectedEntities.size() == 0)
            {
                ImGui::Text("No entity selected");
            }
            else if (s_selectedEntities.size() == 1)
            {
                ImGui::Text("Entity ID: %d", s_selectedEntities[0]);
            }
            else
            {
                ImGui::Text("Multiple entities selected");
            }
        }

        ImGui::End();
    }

    void EditorEntityWindowShutdown()
    {
    }
} // namespace ntt
