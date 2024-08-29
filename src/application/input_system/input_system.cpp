#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/core/logging.hpp>

#define KEY_SIZE 256
#define MOUSE_BUTTON_SIZE 3

#define USE_RAYLIB

#ifdef USE_RAYLIB
#include <raylib.h>
#define CHECK_KEY_STATE(key)                                       \
    if (IsKeyPressed(KEY_##key))                                   \
    {                                                              \
        s_keyStates[Key::NTT_KEY_##key] = InputState::NTT_PRESS;   \
    }                                                              \
    else if (IsKeyDown(KEY_##key))                                 \
    {                                                              \
        s_keyStates[Key::NTT_KEY_##key] = InputState::NTT_DOWN;    \
    }                                                              \
    else if (IsKeyUp(KEY_##key))                                   \
    {                                                              \
        s_keyStates[Key::NTT_KEY_##key] = InputState::NTT_UP;      \
    }                                                              \
    else                                                           \
    {                                                              \
        s_keyStates[Key::NTT_KEY_##key] = InputState::NTT_RELEASE; \
    }

#define CHECK_MOUSE_STATE(button)                                                  \
    if (IsMouseButtonPressed(MOUSE_BUTTON_##button))                               \
    {                                                                              \
        s_mouseStates[MouseButton::NTT_BUTTON_##button] = InputState::NTT_PRESS;   \
    }                                                                              \
    else if (IsMouseButtonDown(MOUSE_BUTTON_##button))                             \
    {                                                                              \
        s_mouseStates[MouseButton::NTT_BUTTON_##button] = InputState::NTT_DOWN;    \
    }                                                                              \
    else if (IsMouseButtonUp(MOUSE_BUTTON_##button))                               \
    {                                                                              \
        s_mouseStates[MouseButton::NTT_BUTTON_##button] = InputState::NTT_UP;      \
    }                                                                              \
    else                                                                           \
    {                                                                              \
        s_mouseStates[MouseButton::NTT_BUTTON_##button] = InputState::NTT_RELEASE; \
    }
#else
#error "Not support other system"
// #define CHECK_KEY_STATE(key)
#endif

namespace ntt::input
{
    namespace
    {
        // InputState s_keyPreStates[KEY_SIZE];
        InputState s_keyStates[KEY_SIZE];

        Position s_mousePrePos;
        Position s_mousePos;
        // InputState s_mousePreStates[KEY_SIZE];
        InputState s_mouseStates[MOUSE_BUTTON_SIZE];
    } // namespace

    void Update(f32 delta)
    {
        CHECK_KEY_STATE(A);
        CHECK_KEY_STATE(B);
        CHECK_KEY_STATE(C);
        CHECK_KEY_STATE(D);
        CHECK_KEY_STATE(E);
        CHECK_KEY_STATE(F);
        CHECK_KEY_STATE(G);
        CHECK_KEY_STATE(H);
        CHECK_KEY_STATE(I);
        CHECK_KEY_STATE(J);
        CHECK_KEY_STATE(K);
        CHECK_KEY_STATE(L);
        CHECK_KEY_STATE(M);
        CHECK_KEY_STATE(N);
        CHECK_KEY_STATE(O);
        CHECK_KEY_STATE(P);
        CHECK_KEY_STATE(Q);
        CHECK_KEY_STATE(R);
        CHECK_KEY_STATE(S);
        CHECK_KEY_STATE(T);
        CHECK_KEY_STATE(U);
        CHECK_KEY_STATE(V);
        CHECK_KEY_STATE(W);
        CHECK_KEY_STATE(X);
        CHECK_KEY_STATE(Y);
        CHECK_KEY_STATE(Z);
        CHECK_KEY_STATE(ZERO);
        CHECK_KEY_STATE(ONE);
        CHECK_KEY_STATE(TWO);
        CHECK_KEY_STATE(THREE);
        CHECK_KEY_STATE(FOUR);
        CHECK_KEY_STATE(FIVE);
        CHECK_KEY_STATE(SIX);
        CHECK_KEY_STATE(SEVEN);
        CHECK_KEY_STATE(EIGHT);
        CHECK_KEY_STATE(NINE);
        CHECK_KEY_STATE(ESCAPE);
        CHECK_KEY_STATE(LEFT_CONTROL);
        CHECK_KEY_STATE(LEFT_SHIFT);
        CHECK_KEY_STATE(LEFT_ALT);
        CHECK_KEY_STATE(RIGHT_CONTROL);
        CHECK_KEY_STATE(RIGHT_SHIFT);
        CHECK_KEY_STATE(RIGHT_ALT);

        CHECK_MOUSE_STATE(LEFT);
        CHECK_MOUSE_STATE(RIGHT);
        CHECK_MOUSE_STATE(MIDDLE);

        s_mousePrePos.pix_x = s_mousePos.pix_x;
        s_mousePrePos.pix_y = s_mousePos.pix_y;
        s_mousePos.pix_x = GetMouseX();
        s_mousePos.pix_y = GetMouseY();
    }

    b8 CheckState(Key key, InputState state)
    {
        return s_keyStates[key] == state;
    }

    b8 CheckState(MouseButton button, InputState state)
    {
        return s_mouseStates[button] == state;
    }

    b8 IsMouseMoving()
    {
        return s_mousePrePos.pix_x != s_mousePos.pix_x || s_mousePrePos.pix_y != s_mousePos.pix_y;
    }

    Position &GetMousePosition()
    {
        return s_mousePos;
    }

    i16 GetMouseScroll()
    {
        return 0;
    }
} // namespace ntt::input
