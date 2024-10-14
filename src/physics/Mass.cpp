#include <NTTEngine/physics/Mass.hpp>
#include "imgui.h"

namespace ntt::physics
{
    void Mass::AddForceConst(position_t x, position_t y)
    {
        force_x = x;
        force_y = y;
        acc_x = force_x / mass;
        acc_y = force_y / mass;
    }

    String Mass::GetName() const
    {
        return "Mass";
    }

    void Mass::FromJSON(const JSON &json)
    {
        mass = json.Get<f32>("mass");
        velocity_x = json.Get<position_t>("velocity_x");
        velocity_y = json.Get<position_t>("velocity_y");
        acc_x = json.Get<position_t>("acc_x");
        acc_y = json.Get<position_t>("acc_y");
        force_x = json.Get<position_t>("force_x");
        force_y = json.Get<position_t>("force_y");
    }

    JSON Mass::ToJSON() const
    {
        JSON json;
        json.Set("mass", mass);
        json.Set("velocity_x", velocity_x);
        json.Set("velocity_y", velocity_y);
        json.Set("acc_x", acc_x);
        json.Set("acc_y", acc_y);
        json.Set("force_x", force_x);
        json.Set("force_y", force_y);
        return json;
    }

    void Mass::OnEditorUpdate(std::function<void()> onChanged, void *data)
    {
        ImGui::Separator();

        if (ImGui::InputFloat("mass", &mass, 0.1f, 1.0f, "%.2f",
                              ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged != nullptr)
            {
                onChanged();
            }
        }

        ImGui::InputFloat("velocity_x", &velocity_x);
        ImGui::InputFloat("velocity_y", &velocity_y);
        ImGui::InputFloat("acc_x", &acc_x);
        ImGui::InputFloat("acc_y", &acc_y);
        ImGui::InputFloat("force_x", &force_x);
        ImGui::InputFloat("force_y", &force_y);

        if (ImGui::Button("Add Force"))
        {
            AddForceConst(1, 1);
        }
    }
} // namespace ntt::physics