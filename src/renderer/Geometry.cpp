#include <NTTEngine/renderer/Geometry.hpp>
#include "imgui.h"
#include <NTTEngine/ecs/ecs.hpp>

namespace ntt
{
    namespace
    {
        List<String> priorities = {
            "PRIORIY_0",
            "PRIORIY_1",
            "PRIORIY_2",
            "PRIORIY_3",
            "PRIORIY_4",
        };
    } // namespace

    String Geometry::GetName() const
    {
        return "Geometry";
    }

    void Geometry::TurnOff()
    {
        ECSSetComponentActive(entity_id, typeid(Geometry), FALSE);
    }

    void Geometry::TurnOn()
    {
        ECSSetComponentActive(entity_id, typeid(Geometry), TRUE);
    }

    JSON Geometry::ToJSON() const
    {
        JSON json("{}");
        json.Set("position", pos.ToJSON());
        json.Set("size", size.ToJSON());
        json.Set("rotation", rotation);
        json.Set("priority", priority);
        json.Set("keepRatio", keepRatio);
        json.Set("color", color.ToJSON());
        return json;
    }

    void Geometry::FromJSON(const JSON &json)
    {
        pos.FromJSON(json.Get<JSON>("position"));
        size.FromJSON(json.Get<JSON>("size"));
        rotation = json.Get<f32>("rotation", 0.0f);
        priority = json.Get<u8>("priority", PRIORITY_0);
        keepRatio = json.Get<b8>("keepRatio", FALSE);
        color.FromJSON(json.Get<JSON>("color"));
    }

    void Geometry::OnEditorUpdate(std::function<void()> onChanged, void *data)
    {
        pos.OnEditorUpdate(onChanged);
        size.OnEditorUpdate(onChanged, keepRatio ? originalSize.width / originalSize.height : -1.0f);
        if (ImGui::InputFloat("rotation",
                              &rotation, 1.0f, 10.f,
                              "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged)
            {
                onChanged();
            }
        }

        ImGui::Checkbox("Keep ratio", &keepRatio);

        if (keepRatio)
        {
            size.height = size.width * originalSize.height / originalSize.width;
        }

        color.OnEditorUpdate(onChanged);

        if (ImGui::BeginCombo("priority", priorities[priority].RawString().c_str()))
        {
            for (auto i = 0; i < priorities.size(); i++)
            {
                b8 isSelected = priority == i;
                if (ImGui::Selectable(priorities[i].RawString().c_str(), isSelected))
                {
                    priority = i;
                    if (onChanged)
                    {
                        onChanged();
                    }
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }
    }
} // namespace ntt
