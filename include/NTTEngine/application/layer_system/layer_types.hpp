#pragma once

namespace ntt
{
    using layer_t = u8;

    /**
     * All supported layer types in the engine, each layer
     *      has the different functionalities and the different
     *      update order.
     */

#define GAME_LAYER 0x00 ///< The main game layer where the physics, rendering
                        ///<     and the game logic are updated.

#define UI_LAYER 0x01 ///< The UI layer where the UI components are updated
                      ///<      like the menu, the HUD, the dialog, etc.
} // namespace ntt
