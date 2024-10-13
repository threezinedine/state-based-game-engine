#include <NTTEngine/renderer/GraphicInterface.hpp>
#include "imgui.h"

namespace ntt::renderer
{
    JSON Grid::ToJSON() const
    {
        JSON json;

        json.Set("row", row);
        json.Set("col", col);

        return json;
    }

    void Grid::FromJSON(const JSON &json)
    {
        row = json.Get<u8>("row");
        col = json.Get<u8>("col");
    }

    void Grid::EditorUpdate(std::function<void()> onChanged)
    {
        static u8 step = 1;
        static u8 step_fast = 2;
        if (ImGui::InputScalar(
                "Row", ImGuiDataType_U8, &row,
                &step, &step_fast, "%d",
                ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged)
            {
                onChanged();
            }
        }

        if (ImGui::InputScalar(
                "Col", ImGuiDataType_U8, &col, &step, &step_fast,
                "%d", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged)
            {
                onChanged();
            }
        }
    }
} // namespace ntt::renderer
