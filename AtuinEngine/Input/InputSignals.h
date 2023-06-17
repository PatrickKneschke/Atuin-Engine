
#pragma once


#include "Core/Util/Types.h"
#include "Core/Util/StringID.h"

#include <string>


namespace Atuin {


enum class Signal : I32 {

    // unknown input
    UNKNOWN = -1,

    // keyboard keys
    SPACE,
    APOSTROPHE,  /* ' */
    COMMA,
    MINUS,
    PERIOD,
    SLASH, /* / */
    NUM0,
    NUM1,
    NUM2,
    NUM3,
    NUM4,
    NUM5,
    NUM6,
    NUM7,
    NUM8,
    NUM9,
    SEMICOLON,
    EQUAL,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    LEFT_BRACKET, /* [ */
    BACKSLASH, /* \ */
    RIGHT_BRACKET, /* ] */
    GRAVE_ACCENT, /* ` */
    ESCAPE,
    ENTER,
    TAB,
    BACKSPACE,
    INSERT,
    DELETE,
    RIGHT,
    LEFT,
    DOWN,
    UP,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    CAPS_LOCK,
    SCROLL_LOCK,
    NUM_LOCK,
    PRINT_SCREEN,
    PAUSE,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    F25,
    KP0,
    KP1,
    KP2,
    KP3,
    KP4,
    KP5,
    KP6,
    KP7,
    KP8,
    KP9,
    KP_DECIMAL,
    KP_DIVIDE,
    KP_MULTIPLY,
    KP_SUBTRACT,
    KP_ADD,
    KP_ENTER,
    KP_EQUAL,
    LEFT_SHIFT,
    LEFT_CTRL,
    LEFT_ALT,
    LEFT_SUPER,
    RIGHT_SHIFT,
    RIGHT_CTRL,
    RIGHT_ALT,
    RIGHT_SUPER,
    MENU,

    // mouse buttons
    MOUSE_1,
    MOUSE_2,
    MOUSE_3,
    MOUSE_4,
    MOUSE_5,
    MOUSE_6,
    MOUSE_7,
    MOUSE_8,

    // mouse move
    MOUSE_MOVE_X,
    MOUSE_MOVE_Y,
    MOUSE_SCROLL_UP,
    MOUSE_SCROLL_DOWN,
    MOUSE_SCROLL_RIGHT,
    MOUSE_SCROLL_LEFT,

    // gamepad button
    GAMEPAD_A,
    GAMEPAD_B,
    GAMEPAD_X,
    GAMEPAD_Y,
    GAMEPAD_LEFT_BUMPER,
    GAMEPAD_RIGHT_BUMPER,
    GAMEPAD_BACK,
    GAMEPAD_START,
    GAMEPAD_GUIDE,
    GAMEPAD_LEFT_THUMB,
    GAMEPAD_RIGHT_THUMB,
    GAMEPAD_DPAD_UP,
    GAMEPAD_DPAD_RIGHT,
    GAMEPAD_DPAD_DOWN,
    GAMEPAD_DPAD_LEFT,

    // gamepad axes
    GAMEPAD_AXIS_LEFT_X,
    GAMEPAD_AXIS_LEFT_Y,
    GAMEPAD_AXIS_RIGHT_X,
    GAMEPAD_AXIS_RIGHT_Y,
    GAMEPAD_AXIS_LEFT_TRIGGER,
    GAMEPAD_AXIS_RIGHT_TRIGGER,

    // number of keys
    COUNT,

    // aliases
    MOUSE_LEFT = MOUSE_1,
    MOUSE_RIGHT = MOUSE_2,
    MOUSE_MIDDLE = MOUSE_3, 

    GAMEPAD_CROSS = GAMEPAD_A,
    GAMEPAD_CIRCLE = GAMEPAD_B,
    GAMEPAD_SQUARE = GAMEPAD_X,
    GAMEPAD_TRIANGLE = GAMEPAD_Y,
};


const std::string ToString(Signal signal);
Signal StringToSignal(std::string_view signalName);

    
} // Atuin
