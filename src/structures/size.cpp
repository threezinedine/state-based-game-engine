#include <NTTEngine/structures/size.hpp>
#include "imgui.h"

namespace ntt
{
    void Size::FromJSON(const JSON &json)
    {
        width = json.Get<ntt_size_t>("width", SIZE_DEFAULT);
        height = json.Get<ntt_size_t>("height", SIZE_DEFAULT);
    }

    JSON Size::ToJSON() const
    {
        JSON json("{}");
        json.Set("width", width);
        json.Set("height", height);
        return json;
    }

    void Size::OnEditorUpdate(std::function<void()> onChanged)
    {
        if (ImGui::InputFloat("width", &width, 1.0f, 10.f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged)
            {
                onChanged();
            }
        }

        if (ImGui::InputFloat("heigth", &height, 1.0f, 10.f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged)
            {
                onChanged();
            }
        }
    }
} // namespace ntt
