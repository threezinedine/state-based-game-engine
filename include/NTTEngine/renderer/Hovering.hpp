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

        Hovering() = default;

        String GetName() const override;

        JSON ToJSON() const override;
        void FromJSON(const JSON &json) override;

        void OnEditorUpdate(std::function<void()> onChanged, void *data) override;
    };
} // namespace ntt
