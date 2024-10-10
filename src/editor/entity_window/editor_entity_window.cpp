#include "editor_entity_window.hpp"
#include "imgui.h"
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

#include "geometry_component.hpp"
#include "texture_component.hpp"
#include "mass_component.hpp"

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

        void OnSceneChanged(event_code_t code, void *sender, const EventContext &context)
        {
            s_selectedEntities.clear();
        }
    }

    void EditorEntityWindowInit()
    {
        s_selectedEntities.clear();

        RegisterEvent(NTT_EDITOR_CHOOSE_ENTITY, OnEntitySelected);
        RegisterEvent(NTT_EDITOR_APPEND_ENTITY, OnEntityAppend);
        RegisterEvent(NTT_SCENE_CHANGED, OnSceneChanged);

        GeometryComponentInit();
        TextureComponentInit();
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
                auto components = ECSGetEntity(s_selectedEntities[0])->components;

                ImGui::Text("Entity ID: %d", s_selectedEntities[0]);
                ImGui::Separator();
                if (ImGui::TreeNodeEx("Components", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    for (auto pair : components)
                    {
                        String activeName = "";
                        if (pair.first == typeid(Geometry))
                        {
                            GeometryComponentDraw(
                                std::static_pointer_cast<Geometry>(pair.second));

                            activeName = "Geometry Active";
                        }
                        else if (pair.first == typeid(TextureComponent))
                        {
                            TextureComponentDraw(
                                std::static_pointer_cast<TextureComponent>(pair.second));

                            activeName = "Texture Active";
                        }
                        else if (pair.first == typeid(Mass))
                        {
                            MassComponentDraw(
                                std::static_pointer_cast<Mass>(pair.second));
                            activeName = "Mass Active";
                        }

                        b8 active = pair.second->active;

                        if (activeName != "")
                        {
                            active = pair.second->active;
                            if (ImGui::Checkbox(activeName.RawString().c_str(), &active))
                            {
                                ECSSetComponentActive(
                                    s_selectedEntities[0],
                                    pair.first,
                                    active);
                            }
                        }
                    }
                    ImGui::TreePop();
                }
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
        GeometryComponentShutdown();
        TextureComponentShutdown();
    }
} // namespace ntt
