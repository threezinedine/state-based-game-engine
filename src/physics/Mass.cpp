#include <NTTEngine/physics/Mass.hpp>
#include "imgui.h"

namespace ntt::physics
{
    void Mass::AddForceConst(position_t forceX, position_t forceY)
    {
        acc_x = forceX / mass;
        acc_y = forceY / mass;
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
    }

    JSON Mass::ToJSON() const
    {
        JSON json;
        json.Set("mass", mass);
        json.Set("velocity_x", velocity_x);
        json.Set("velocity_y", velocity_y);
        json.Set("acc_x", acc_x);
        json.Set("acc_y", acc_y);
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

        if (ImGui::InputFloat("velocity_x", &velocity_x,
                              0.1f, 1.0f, "%.2f",
                              ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged != nullptr)
            {
                onChanged();
            }
        }

        if (ImGui::InputFloat("velocity_y", &velocity_y,
                              0.1f, 1.0f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged != nullptr)
            {
                onChanged();
            }
        }

        if (ImGui::InputFloat("acc_x", &acc_x,
                              0.01f, 0.1f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged != nullptr)
            {
                onChanged();
            }
        }

        if (ImGui::InputFloat("acc_y", &acc_y,
                              0.01f, 0.10f, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged != nullptr)
            {
                onChanged();
            }
        }
    }
} // namespace ntt::physics