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
        static ntt_size_t tempWidth = width;
        static ntt_size_t tempHeight = height;

        tempHeight = height;
        tempWidth = width;

        ImGui::InputFloat("width", &tempWidth, 1.0f, 10.f, "%.3f", 0);
        ImGui::InputFloat("heigth", &tempHeight, 1.0f, 10.f, "%.3f", 0);

        if (tempWidth != width || tempHeight != height)
        {
            width = tempWidth;
            height = tempHeight;
            if (onChanged)
            {
                onChanged();
            }
        }
    }
} // namespace ntt
