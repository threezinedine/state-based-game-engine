#include "imgui.h"
#include "rlImGui.h"
#include "editor_scene_window.hpp"
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/ecs/ecs.hpp>

namespace ntt
{
    using namespace event;
    using namespace ecs;

    List<entity_id_t> s_entities;

    namespace
    {
        void OnCreateEntity(event_code_t code, void *sender, const EventContext context)
        {
            entity_id_t id = context.u32_data[0];
            s_entities.push_back(id);
        }

        void OnDestroyEntity(event_code_t code, void *sender, const EventContext context)
        {
            entity_id_t id = context.u32_data[0];
            s_entities.RemoveItem(id);
        }
    } // namespac

    void EditorSceneWindowInit()
    {
        RegisterEvent(NTT_ENTITY_CREATED, OnCreateEntity);
        RegisterEvent(NTT_ENTITY_DESTROYED, OnDestroyEntity);
    }

    void EditorSceneWindowUpdate(b8 *p_open, b8 isRunning)
    {
        if (ImGui::Begin("Scene", p_open, isRunning ? ImGuiWindowFlags_NoInputs : 0))
        {
            if (ImGui::TreeNode("Entities"))
            {
                for (auto id : s_entities)
                {
                    auto entityInfo = ECSGetEntity(id);
                    ImGui::Text("Entity %s", entityInfo->name.RawString().c_str());
                }
                ImGui::TreePop();
            }
        }

        ImGui::End();
    }

    void EditorSceneWindowShutdown()
    {
    }
} // namespace ntt
