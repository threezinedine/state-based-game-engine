#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <cstring>

#define KEY_SIZE 256
#define MOUSE_BUTTON_SIZE 3

#define USE_RAYLIB

#ifdef USE_RAYLIB
#include <raylib.h>
#define CHECK_KEY_STATE(key)                                       \
    ctx.u8_data[0] = Key::NTT_KEY_##key;                           \
    if (IsKeyPressed(KEY_##key))                                   \
    {                                                              \
        s_keyStates[Key::NTT_KEY_##key] = InputState::NTT_PRESS;   \
        TriggerEvent(NTT_EVENT_KEY_PRESSED, nullptr, ctx);         \
    }                                                              \
    else if (IsKeyReleased(KEY_##key))                             \
    {                                                              \
        s_keyStates[Key::NTT_KEY_##key] = InputState::NTT_RELEASE; \
        TriggerEvent(NTT_EVENT_KEY_RELEASED, nullptr, ctx);        \
    }                                                              \
    else if (IsKeyUp(KEY_##key))                                   \
    {                                                              \
        s_keyStates[Key::NTT_KEY_##key] = InputState::NTT_UP;      \
    }                                                              \
    else                                                           \
    {                                                              \
        s_keyStates[Key::NTT_KEY_##key] = InputState::NTT_DOWN;    \
    }

#define CHECK_MOUSE_STATE(button)                                                  \
    ctx.u8_data[0] = MouseButton::NTT_BUTTON_##button;                             \
    if (IsMouseButtonPressed(MOUSE_BUTTON_##button))                               \
    {                                                                              \
        s_mouseStates[MouseButton::NTT_BUTTON_##button] = InputState::NTT_PRESS;   \
        TriggerEvent(NTT_EVENT_MOUSE_PRESS, nullptr, ctx);                         \
    }                                                                              \
    else if (IsMouseButtonReleased(MOUSE_BUTTON_##button))                         \
    {                                                                              \
        s_mouseStates[MouseButton::NTT_BUTTON_##button] = InputState::NTT_RELEASE; \
        TriggerEvent(NTT_EVENT_MOUSE_RELEASE, nullptr, ctx);                       \
    }                                                                              \
    else if (IsMouseButtonUp(MOUSE_BUTTON_##button))                               \
    {                                                                              \
        s_mouseStates[MouseButton::NTT_BUTTON_##button] = InputState::NTT_UP;      \
    }                                                                              \
    else                                                                           \
    {                                                                              \
        s_mouseStates[MouseButton::NTT_BUTTON_##button] = InputState::NTT_DOWN;    \
    }
#else
#error "Not support other system"
// #define CHECK_KEY_STATE(key)
#endif

#define GET_KEY_NAME(key)    \
    case Key::NTT_KEY_##key: \
        return #key;

namespace ntt::input
{
    using namespace log;
    using namespace event;

    namespace
    {
        // InputState s_keyPreStates[KEY_SIZE];
        InputState s_keyStates[KEY_SIZE];

        Position s_mousePrePos;
        Position s_mousePos;
        // InputState s_mousePreStates[KEY_SIZE];
        InputState s_mouseStates[MOUSE_BUTTON_SIZE];
    } // namespace

    void InputUpdate(f32 delta)
    {
        EventContext ctx;
        memset(&ctx, 0, sizeof(EventContext));

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
        CHECK_KEY_STATE(SPACE);
        CHECK_KEY_STATE(ENTER);
        CHECK_KEY_STATE(UP);
        CHECK_KEY_STATE(DOWN);
        CHECK_KEY_STATE(LEFT);
        CHECK_KEY_STATE(RIGHT);

        CHECK_MOUSE_STATE(LEFT);
        CHECK_MOUSE_STATE(RIGHT);
        CHECK_MOUSE_STATE(MIDDLE);

        s_mousePrePos.x = s_mousePos.x;
        s_mousePrePos.y = s_mousePos.y;
        s_mousePos.x = static_cast<position_t>(::GetMouseX());
        s_mousePos.y = static_cast<position_t>(::GetMouseY());
    }

    const char *GetKeyName(Key key)
    {
        switch (key)
        {
            GET_KEY_NAME(A);
            GET_KEY_NAME(B);
            GET_KEY_NAME(C);
            GET_KEY_NAME(D);
            GET_KEY_NAME(E);
            GET_KEY_NAME(F);
            GET_KEY_NAME(G);
            GET_KEY_NAME(H);
            GET_KEY_NAME(I);
            GET_KEY_NAME(J);
            GET_KEY_NAME(K);
            GET_KEY_NAME(L);
            GET_KEY_NAME(M);
            GET_KEY_NAME(N);
            GET_KEY_NAME(O);
            GET_KEY_NAME(P);
            GET_KEY_NAME(Q);
            GET_KEY_NAME(R);
            GET_KEY_NAME(S);
            GET_KEY_NAME(T);
            GET_KEY_NAME(U);
            GET_KEY_NAME(V);
            GET_KEY_NAME(W);
            GET_KEY_NAME(X);
            GET_KEY_NAME(Y);
            GET_KEY_NAME(Z);
            GET_KEY_NAME(ZERO);
            GET_KEY_NAME(ONE);
            GET_KEY_NAME(TWO);
            GET_KEY_NAME(THREE);
            GET_KEY_NAME(FOUR);
            GET_KEY_NAME(FIVE);
            GET_KEY_NAME(SIX);
            GET_KEY_NAME(SEVEN);
            GET_KEY_NAME(EIGHT);
            GET_KEY_NAME(NINE);
            GET_KEY_NAME(ESCAPE);
            GET_KEY_NAME(LEFT_CONTROL);
            GET_KEY_NAME(LEFT_SHIFT);
            GET_KEY_NAME(LEFT_ALT);
            GET_KEY_NAME(RIGHT_CONTROL);
            GET_KEY_NAME(RIGHT_SHIFT);
            GET_KEY_NAME(RIGHT_ALT);
            GET_KEY_NAME(SPACE);
            GET_KEY_NAME(ENTER);
            GET_KEY_NAME(UP);
            GET_KEY_NAME(DOWN);
            GET_KEY_NAME(LEFT);
            GET_KEY_NAME(RIGHT);
        default:
            return "Key is not supported string version";
        }
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
        return s_mousePrePos.x != s_mousePos.x || s_mousePrePos.y != s_mousePos.y;
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
