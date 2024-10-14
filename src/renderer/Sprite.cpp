#include <NTTEngine/renderer/Sprite.hpp>
#include "imgui.h"

namespace ntt::renderer
{
    String Sprite::GetName() const
    {
        return "Sprite";
    }

    void Sprite::FromJSON(const JSON &json)
    {
        changePerMilis = json.Get<f32>("changePerMilis");
        currentCell = json.Get<u32>("currentCell");
        cells.clear();

        List<JSON> cellJSONs = json.GetList<JSON>("cells");

        for (auto cell : cellJSONs)
        {
            cells.push_back({cell.Get<u8>("x"), cell.Get<u8>("y")});
        }
    }

    JSON Sprite::ToJSON() const
    {
        JSON json;
        json.Set("changePerMilis", changePerMilis);
        json.Set("currentCell", currentCell);

        List<JSON> cellList;
        for (auto cell : cells)
        {
            JSON cellJson;
            cellJson.Set("x", cell.first);
            cellJson.Set("y", cell.second);
            cellList.push_back(cellJson);
        }

        json.Set("cells", JSON::FromList(cellList));
        return json;
    }

    void Sprite::OnEditorUpdate(std::function<void()> onChanged, void *data)
    {
        static u32 step = 1;

        if (ImGui::InputFloat("Change Per Milis", &changePerMilis, 0.1f, 1.0f, "%.2f",
                              ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged != nullptr)
            {
                onChanged();
            }
        }

        if (cells.size() != 0)
        {
            if (ImGui::TreeNode("Cells"))
            {
                for (auto cell : cells)
                {
                    ImGui::Text(format("X: {}, Y: {}",
                                       cell.first, cell.second)
                                    .RawString()
                                    .c_str());
                }

                ImGui::TreePop();
            }
        }
        else
        {
            ImGui::Text("No cells");
        }

        if (ImGui::Button("Add cell"))
        {
            ImGui::OpenPopup("add_cell");
        }

        if (ImGui::BeginPopup("add_cell"))
        {
            static u8 x = 0;
            static u8 y = 0;

            ImGui::InputScalar("X", ImGuiDataType_U8, &x, &step, &step, "%u");
            ImGui::InputScalar("Y", ImGuiDataType_U8, &y, &step, &step, "%u");

            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Save"))
            {
                cells.push_back({x, y});

                if (onChanged != nullptr)
                {
                    onChanged();
                }

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Separator();

        if (ImGui::InputScalar("Current cell", ImGuiDataType_U32,
                               &currentCell, &step, &step, "%u", ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (onChanged != nullptr)
            {
                onChanged();
            }
        }
    }
} // namespace ntt::renderer
