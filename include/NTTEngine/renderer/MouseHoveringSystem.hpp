#pragma once
#include "renderer.hpp"

namespace ntt::renderer
{
    void MouseHoveringSystemInit();

    void MouseHoveringSystemFunc(f32 delta, entity_id_t id);

    /**
     * Actually handle the mouse hovering system update.
     * Be called in the game loop.
     */
    void MouseHoveringSystemUpdate(f32 delta);

    void MouseHoveringSystemShutdown();
} // namespace ntt::renderer