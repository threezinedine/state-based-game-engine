#include <NTTEngine/renderer/Grid.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include "imgui.h"
#include <NTTEngine/resources/ResourceManager.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/editor/editor_clipboard.hpp>

namespace ntt
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

    void Grid::SetResourceName(const String &resourceName)
    {
        m_resourceName = resourceName;
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

        if (ImGui::Button("More"))
        {
            ImGui::OpenPopup("More");
        }

        if (m_resourceName != "")
        {
            if (ImGui::BeginPopup("More"))
            {
                if (ImGui::MenuItem("Clipboard"))
                {
                    Grid grid = EditorClipboard_GetGrid();
                    row = grid.row;
                    col = grid.col;
                    if (onChanged)
                    {
                        onChanged();
                    }
                }

                ImGui::EndPopup();
            }
        }
    }
} // namespace ntt
