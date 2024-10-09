#include "geometry_component.hpp"
#include "imgui.h"
#include <NTTEngine/platforms/application.hpp>

namespace ntt
{
    void TextureComponentDraw(Ref<TextureComponent> texture)
    {
        static const char *u8one = (const char *)1;
        ImGui::Separator();
        auto windowSize = GetWindowSize();
        ImGui::Text(format("Texture ID: %d", texture->id).RawString().c_str());
        ImGui::InputScalar("Row Index", ImGuiDataType_U8,
                           &texture->currentCell.row, &u8one, NULL, "%d", 0);
        ImGui::InputScalar("Col Index", ImGuiDataType_U8,
                           &texture->currentCell.col, &u8one, NULL, "%d", 0);
    }
} // namespace ntt
