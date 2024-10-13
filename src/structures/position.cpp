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
        static position_t tempX = x;
        static position_t tempY = y;

        tempX = x;
        tempY = y;
        ImGui::InputFloat("x", &tempX, 1.0f, 10.f, "%.3f", 0);
        ImGui::InputFloat("y", &tempY, 1.0f, 10.f, "%.3f", 0);

        if (tempX != x || tempY != y)
        {
            x = tempX;
            y = tempY;
            if (onChanged)
            {
                onChanged();
            }
        }
    }
} // namespace ntt
