#include <NTTEngine/renderer/Geometry.hpp>
#include "imgui.h"

namespace ntt::renderer
{
    String Geometry::GetName() const
    {
        return "Geometry";
    }

    JSON Geometry::ToJSON() const
    {
        JSON json("{}");
        json.Set("position", pos.ToJSON());
        json.Set("size", size.ToJSON());
        json.Set("rotation", rotation);
        json.Set("priority", priority);
        json.Set("color", color.ToJSON());
        return json;
    }

    void Geometry::FromJSON(const JSON &json)
    {
        pos.FromJSON(json.Get<JSON>("position"));
        size.FromJSON(json.Get<JSON>("size"));
        rotation = json.Get<f32>("rotation", 0.0f);
        priority = json.Get<u8>("priority", PRIORITY_0);
        color.FromJSON(json.Get<JSON>("color"));
    }

    void Geometry::OnEditorUpdate(std::function<void()> onChanged)
    {
        pos.OnEditorUpdate(onChanged);
        size.OnEditorUpdate(onChanged);
        if (ImGui::InputFloat("rotation",
                              &rotation, 1.0f, 10.f,
                              "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged)
            {
                onChanged();
            }
        }
        color.OnEditorUpdate(onChanged);
    }
} // namespace ntt::renderer
