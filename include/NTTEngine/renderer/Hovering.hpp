#pragma once
#include "renderer.hpp"
#include <NTTEngine/core/object.hpp>

namespace ntt
{
    struct HoveringContext
    {
        b8 goNext = TRUE; ///< true if the lower pirority entity should be which
                          ///< is hovered by the mouse can be called, false otherwise.

        HoveringContext() = default;
        HoveringContext(b8 goNext)
            : goNext(goNext)
        {
        }

        void StopPass()
        {
            goNext = FALSE;
        }
    };

    using HoveringCallback = std::function<void(HoveringContext &)>;

    struct Hovering : public ComponentBase
    {
        HoveringCallback onEnterCallback;
        HoveringCallback callback;
        HoveringCallback onExitCallback;
        b8 cursor = FALSE;    ///< TRUE if the cursor is hovering the entity, FALSE otherwise.
        Grid hoveredCell;     ///< The texture cell which will be displayed when the entity is hovered
        Grid prevHoveredCell; ///< Temp storage variable for the previous hovered cell

        Hovering()
            : onEnterCallback(nullptr), callback(nullptr), onExitCallback(nullptr),
              hoveredCell({255, 255}), prevHoveredCell({255, 255})
        {
        }

        String GetName() const override;

        JSON ToJSON() const override;
        void FromJSON(const JSON &json) override;

        void OnEditorUpdate(std::function<void()> onChanged, void *data) override;
    };
} // namespace ntt
