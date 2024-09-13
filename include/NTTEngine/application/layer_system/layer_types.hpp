#pragma once

namespace ntt
{
    /**
     * All supported layer types in the engine, each layer
     *      has the different functionalities and the different
     *      update order.
     */
    enum LayerType
    {
        GAME_LAYER = 0x00, ///< The main game layer where the physics, rendering
                           ///<     and the game logic are updated.

        UI_LAYER = 0x01, ///< The UI layer where the UI components are updated
                         ///<      like the menu, the HUD, the dialog, etc.

        DEBUG_LAYER = 0x02, ///< The debug layer where the debug information is
                            ///<      updated like the FPS, the memory usage, etc.
                            ///< In this layer, user can see the information of the
                            ///<      selected entity, the system, the component, etc.
    };
} // namespace ntt
