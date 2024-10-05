#pragma once
#include <NTTEngine/defines.hpp>

namespace ntt
{
    using layer_t = u8;

    /**
     * All supported layer types in the engine, each layer
     *      has the different functionalities and the different
     *      update order.
     *
     * There are 9 custom slot for the UI layers. The priorities of
     *      the UI_LAYER_Xs are same, and only 1 layer can be visible
     *      at the same time.
     *
     * Stack: GAME_LAYER -> UI_LAYER_X -> EDITOR_LAYER
     */

#define GAME_LAYER 0x00 ///< The main game layer where the physics, rendering
                        ///<     and the game logic are updated.

#define UI_LAYER_0 0x01 ///< First slot for the UI layer
#define UI_LAYER_1 0x02 ///< Second slot for the UI layer
#define UI_LAYER_2 0x03 ///< Third slot for the UI layer
#define UI_LAYER_3 0x04 ///< Fourth slot for the UI layer
#define UI_LAYER_4 0x05 ///< Fifth slot for the UI layer
#define UI_LAYER_5 0x06 ///< Sixth slot for the UI layer
#define UI_LAYER_6 0x07 ///< Seventh slot for the UI layer
#define UI_LAYER_7 0x08 ///< Eighth slot for the UI layer
#define UI_LAYER_8 0x09 ///< Ninth slot for the UI layer

#define EDITOR_LAYER 0x0A ///< The debug layer where the all buttons which provide
                          ///<    the debugging functionalities are updated.

#define MAX_LAYERS (EDITOR_LAYER + 1)
} // namespace ntt