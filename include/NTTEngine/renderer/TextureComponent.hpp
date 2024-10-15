#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include "GraphicInterface.hpp"

namespace ntt
{
    /**
     * Store the information about the segment of the texture
     *      which is needed to draw on the screen.
     */
    struct TextureComponent : public ComponentBase
    {
        // resource_id_t id;
        String resourceName;
        Grid currentCell;
        Grid textureGrid;
        String tooltip;

        TextureComponent(const String &resourceName = "",
                         u8 rowIndex = 0,
                         u8 colIndex = 0,
                         const String &tooltip = "")
            : resourceName(resourceName),
              currentCell(rowIndex, colIndex),
              tooltip(tooltip)
        {
        }

        resource_id_t GetTextureID() const;
        String ResourceName() const;

        String GetName() const override;

        JSON ToJSON() const override;
        void FromJSON(const JSON &json) override;

        void OnEditorUpdate(std::function<void()> onChanged = nullptr, void *data = nullptr) override;
    };
} // namespace ntt
