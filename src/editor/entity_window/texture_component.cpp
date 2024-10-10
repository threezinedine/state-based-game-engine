#include "geometry_component.hpp"
#include "imgui.h"
#include <NTTEngine/platforms/application.hpp>
#include <NTTEngine/core/utils.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <NTTEngine/resources/ResourceManager.hpp>
#include <NTTEngine/structures/string.hpp>

namespace ntt
{
    using namespace event;

    namespace
    {
    } // namespace

    void TextureComponentInit()
    {
    }

    void TextureComponentDraw(Ref<TextureComponent> texture)
    {
        auto maxRow = texture->textureGrid.row;
        auto maxCol = texture->textureGrid.col;

        static const char *u8one = (const char *)1;
        ImGui::Separator();
        auto windowSize = GetWindowSize();

        ImGui::Text(format("Texture ID: {}", texture->id).RawString().c_str());

        ImGui::InputScalar(format("Row Index - Max {}", maxRow - 1).RawString().c_str(),
                           ImGuiDataType_U8,
                           &texture->currentCell.row, &u8one, NULL, "%d", 0);
        ImGui::InputScalar(format("Col Index - Max {}", maxCol - 1).RawString().c_str(),
                           ImGuiDataType_U8,
                           &texture->currentCell.col, &u8one, NULL, "%d", 0);

        texture->currentCell.row = GetMin(static_cast<u8>(maxRow - 1), texture->currentCell.row);
        texture->currentCell.col = GetMin(static_cast<u8>(maxCol - 1), texture->currentCell.col);
    }

    void TextureComponentShutdown()
    {
    }
} // namespace ntt
