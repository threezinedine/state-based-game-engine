#pragma once

#include "renderer.hpp"

namespace ntt::renderer
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
    };
} // namespace ntt::renderer
