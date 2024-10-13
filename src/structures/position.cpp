#include <NTTEngine/structures/position.hpp>
#include "imgui.h"

namespace ntt
{
    void Position::FromJSON(const JSON &json)
    {
        x = json.Get<position_t>("x", POSITION_DEFAULT);
        y = json.Get<position_t>("y", POSITION_DEFAULT);
    }

    JSON Position::ToJSON() const
    {
        JSON json("{}");
        json.Set("x", x);
        json.Set("y", y);
        return json;
    }

    void Position::OnEditorUpdate(std::function<void()> onChanged)
    {
        if (ImGui::InputFloat("x", &x, 1.0f, 10.f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged)
            {
                onChanged();
            }
        }
        if (ImGui::InputFloat("y", &y, 1.0f, 10.f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged)
            {
                onChanged();
            }
        }
    }
} // namespace ntt
