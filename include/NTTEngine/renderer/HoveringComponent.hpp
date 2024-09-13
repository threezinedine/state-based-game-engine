#pragma once
#include "renderer.hpp"

namespace ntt::renderer
{
    using HoveringCallback = std::function<void()>;

    struct HoveringComponent : public ComponentBase
    {
        HoveringCallback callback;

        HoveringComponent() = default;
        HoveringComponent(HoveringCallback callback)
            : callback(callback)
        {
        }
    };
} // namespace ntt::renderer