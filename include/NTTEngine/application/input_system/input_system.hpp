#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/position.hpp>

#define KEY_DEFINE(key, value) NTT_KEY_##key = value

/**
 * This module is used for detecting user input
 *      like mouse, keyboard, and gamepad input.
 *
 * It also provides the way to bind the input to
 *      an ai system
 *
 */
namespace ntt::input
{
    /**
     * All the keys that this engine supports
     *      and their values it suports
     *      only 8-bit values
     *
     * If wanna change the value to 16-bit (or more)
     *      then in the `input_system.cpp` must
     *      change the trigger event to pass the
     *      context with the u16_data
     */
    enum Key : u8
    {
        KEY_DEFINE(A, 0x00),
        KEY_DEFINE(B, 0x01),
        KEY_DEFINE(C, 0x02),
        KEY_DEFINE(D, 0x03),
        KEY_DEFINE(E, 0x04),
        KEY_DEFINE(F, 0x05),
        KEY_DEFINE(G, 0x06),
        KEY_DEFINE(H, 0x07),
        KEY_DEFINE(I, 0x08),
        KEY_DEFINE(J, 0x09),
        KEY_DEFINE(K, 0x0A),
        KEY_DEFINE(L, 0x0B),
        KEY_DEFINE(M, 0x0C),
        KEY_DEFINE(N, 0x0D),
        KEY_DEFINE(O, 0x0E),
        KEY_DEFINE(P, 0x0F),
        KEY_DEFINE(Q, 0x10),
        KEY_DEFINE(R, 0x11),
        KEY_DEFINE(S, 0x12),
        KEY_DEFINE(T, 0x13),
        KEY_DEFINE(U, 0x14),
        KEY_DEFINE(V, 0x15),
        KEY_DEFINE(W, 0x16),
        KEY_DEFINE(X, 0x17),
        KEY_DEFINE(Y, 0x18),
        KEY_DEFINE(Z, 0x19),
        KEY_DEFINE(ZERO, 0x1A),
        KEY_DEFINE(ONE, 0x1B),
        KEY_DEFINE(TWO, 0x1C),
        KEY_DEFINE(THREE, 0x1D),
        KEY_DEFINE(FOUR, 0x1E),
        KEY_DEFINE(FIVE, 0x1F),
        KEY_DEFINE(SIX, 0x20),
        KEY_DEFINE(SEVEN, 0x21),
        KEY_DEFINE(EIGHT, 0x22),
        KEY_DEFINE(NINE, 0x23),
        KEY_DEFINE(ESCAPE, 0x24),
        KEY_DEFINE(LEFT_CONTROL, 0x25),
        KEY_DEFINE(LEFT_SHIFT, 0x26),
        KEY_DEFINE(LEFT_ALT, 0x27),
        KEY_DEFINE(RIGHT_CONTROL, 0x28),
        KEY_DEFINE(RIGHT_SHIFT, 0x29),
        KEY_DEFINE(RIGHT_ALT, 0x2A),
    };

    /**
     * Get the raw string version
     *      the the key name
     *
     * @param key The key
     */
    const char *GetKeyName(Key key);

    /**
     * The button of the mouse
     */
    enum MouseButton
    {
        NTT_BUTTON_LEFT = 0x00,
        NTT_BUTTON_RIGHT = 0x01,
        NTT_BUTTON_MIDDLE = 0x02,
    };

    /**
     * Store all the posible state of the button
     *      or keyboard
     */
    enum InputState
    {
        NTT_RELEASE = 0x00,
        NTT_PRESS = 0x01,
        NTT_UP = 0x02,
        NTT_DOWN = 0x03,
    };

    /**
     * Should be call at the loop of
     *      the application, which will
     *      update the input system's internal
     *      data.
     *
     * @param delta The time between the last frame
     */
    void Update(f32 delta);

    /**
     * Check whether the key is in
     *      the state or not
     */
    b8 CheckState(Key key, InputState state);

    /**
     * Check whether the mouse button is in
     *      the state or not
     */
    b8 CheckState(MouseButton button, InputState);

    /**
     * Check if mouse is moving
     */
    b8 IsMouseMoving();

    /**
     * Get the current mouse position
     */
    Position &GetMousePosition();

    /**
     * Get scroll value of the mouse
     */
    i16 GetMouseScroll();
} // namespace ntt::input
