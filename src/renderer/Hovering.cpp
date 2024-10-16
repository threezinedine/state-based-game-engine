#include <NTTEngine/renderer/Hovering.hpp>
#include "imgui.h"

namespace ntt
{
    String Hovering::GetName() const
    {
        return "Hovering";
    }

    JSON Hovering::ToJSON() const
    {
        JSON json("{}");
        json.Set("hoveredCell", hoveredCell.ToJSON());
        json.Set("cursor", cursor);
        return json;
    }

    void Hovering::FromJSON(const JSON &json)
    {
        hoveredCell.FromJSON(json.Get<JSON>("hoveredCell"));
        cursor = json.Get<b8>("cursor", FALSE);
    }

    void Hovering::OnEditorUpdate(std::function<void()> onChanged, void *data)
    {
        ImGui::Text("Hovered Cell");
        hoveredCell.SetResourceName("ok");
        hoveredCell.EditorUpdate(onChanged);

        if (ImGui::Checkbox("Cursor", &cursor))
        {
            if (onChanged)
            {
                onChanged();
            }
        }
    }
} // namespace ntt
