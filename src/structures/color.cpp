#include <NTTEngine/structures/color.hpp>
#include "imgui.h"

namespace ntt
{
    JSON RGBAColor::ToJSON() const
    {
        JSON json("{}");
        json.Set("r", r);
        json.Set("g", g);
        json.Set("b", b);
        json.Set("a", a);
        return json;
    }

    void RGBAColor::FromJSON(const JSON &json)
    {
        r = json.Get<u8>("r", 0);
        g = json.Get<u8>("g", 0);
        b = json.Get<u8>("b", 0);
        a = json.Get<u8>("a", 255);
    }

    void RGBAColor::OnEditorUpdate(std::function<void()> onChanged)
    {
        static ImVec4 tempColor = ImVec4(0, 0, 0, 0);

        tempColor.x = r / 255.0f;
        tempColor.y = g / 255.0f;
        tempColor.z = b / 255.0f;
        tempColor.w = a / 255.0f;

        if (ImGui::ColorEdit4("Color", &tempColor.x, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            r = static_cast<u8>(tempColor.x * 255);
            g = static_cast<u8>(tempColor.y * 255);
            b = static_cast<u8>(tempColor.z * 255);
            a = static_cast<u8>(tempColor.w * 255);

            if (onChanged)
            {
                onChanged();
            }
        }
    }
} // namespace ntt
