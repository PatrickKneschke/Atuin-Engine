
#pragma once


#include "InputSignals.h"

#include "GLFW/glfw3.h"


namespace Atuin {


Signal GlfwKeyCode(int glfwKey) {

    Signal signal;
    switch (glfwKey)
    {
        case GLFW_KEY_SPACE :
            signal = Signal::SPACE;
            break;
        case GLFW_KEY_APOSTROPHE :
            signal = Signal::APOSTROPHE;
            break;
        case GLFW_KEY_COMMA :
            signal = Signal::COMMA;
            break;
        case GLFW_KEY_MINUS :
            signal = Signal::MINUS;
            break;
        case GLFW_KEY_PERIOD :
            signal = Signal::PERIOD;
            break;
        case GLFW_KEY_SLASH :
            signal = Signal::SLASH;
            break;
        case GLFW_KEY_0 :
            signal = Signal::NUM0;
            break;
        case GLFW_KEY_1 :
            signal = Signal::NUM1;
            break;
        case GLFW_KEY_2 :
            signal = Signal::NUM2;
            break;
        case GLFW_KEY_3 :
            signal = Signal::NUM3;
            break;
        case GLFW_KEY_4 :
            signal = Signal::NUM4;
            break;
        case GLFW_KEY_5 :
            signal = Signal::NUM5;
            break;
        case GLFW_KEY_6 :
            signal = Signal::NUM6;
            break;
        case GLFW_KEY_7 :
            signal = Signal::NUM7;
            break;
        case GLFW_KEY_8 :
            signal = Signal::NUM8;
            break;
        case GLFW_KEY_9 :
            signal = Signal::NUM9;
            break;
        case GLFW_KEY_SEMICOLON :
            signal = Signal::SEMICOLON;
            break;
        case GLFW_KEY_EQUAL :
            signal = Signal::EQUAL;
            break;
        case GLFW_KEY_A :
            signal = Signal::A;
            break;
        case GLFW_KEY_B :
            signal = Signal::B;
            break;
        case GLFW_KEY_C :
            signal = Signal::C;
            break;
        case GLFW_KEY_D :
            signal = Signal::D;
            break;
        case GLFW_KEY_E :
            signal = Signal::E;
            break;
        case GLFW_KEY_F :
            signal = Signal::F;
            break;
        case GLFW_KEY_G :
            signal = Signal::G;
            break;
        case GLFW_KEY_H :
            signal = Signal::H;
            break;
        case GLFW_KEY_I :
            signal = Signal::I;
            break;
        case GLFW_KEY_J :
            signal = Signal::J;
            break;
        case GLFW_KEY_K :
            signal = Signal::K;
            break;
        case GLFW_KEY_L :
            signal = Signal::L;
            break;
        case GLFW_KEY_M :
            signal = Signal::M;
            break;
        case GLFW_KEY_N :
            signal = Signal::N;
            break;
        case GLFW_KEY_O :
            signal = Signal::O;
            break;
        case GLFW_KEY_P :
            signal = Signal::P;
            break;
        case GLFW_KEY_Q :
            signal = Signal::Q;
            break;
        case GLFW_KEY_R :
            signal = Signal::R;
            break;
        case GLFW_KEY_S :
            signal = Signal::S;
            break;
        case GLFW_KEY_T :
            signal = Signal::T;
            break;
        case GLFW_KEY_U :
            signal = Signal::U;
            break;
        case GLFW_KEY_V :
            signal = Signal::V;
            break;
        case GLFW_KEY_W :
            signal = Signal::W;
            break;
        case GLFW_KEY_X :
            signal = Signal::X;
            break;
        case GLFW_KEY_Y :
            signal = Signal::Y;
            break;
        case GLFW_KEY_Z :
            signal = Signal::Z;
            break;
        case GLFW_KEY_LEFT_BRACKET :
            signal = Signal::LEFT_BRACKET;
            break;
        case GLFW_KEY_BACKSLASH :
            signal = Signal::BACKSLASH;
            break;
        case GLFW_KEY_RIGHT_BRACKET :
            signal = Signal::RIGHT_BRACKET;
            break;
        case GLFW_KEY_GRAVE_ACCENT :
            signal = Signal::GRAVE_ACCENT;
            break;
        case GLFW_KEY_ESCAPE :
            signal = Signal::ESCAPE;
            break;
        case GLFW_KEY_ENTER :
            signal = Signal::ENTER;
            break;
        case GLFW_KEY_TAB :
            signal = Signal::TAB;
            break;
        case GLFW_KEY_BACKSPACE :
            signal = Signal::BACKSPACE;
            break;
        case GLFW_KEY_INSERT :
            signal = Signal::INSERT;
            break;
        case GLFW_KEY_DELETE :
            signal = Signal::DELETE;
            break;
        case GLFW_KEY_RIGHT :
            signal = Signal::RIGHT;
            break;
        case GLFW_KEY_LEFT :
            signal = Signal::LEFT;
            break;
        case GLFW_KEY_DOWN :
            signal = Signal::DOWN;
            break;
        case GLFW_KEY_UP :
            signal = Signal::UP;
            break;
        case GLFW_KEY_PAGE_UP :
            signal = Signal::PAGE_UP;
            break;
        case GLFW_KEY_PAGE_DOWN :
            signal = Signal::PAGE_DOWN;
            break;
        case GLFW_KEY_HOME :
            signal = Signal::HOME;
            break;
        case GLFW_KEY_END :
            signal = Signal::END;
            break;
        case GLFW_KEY_CAPS_LOCK :
            signal = Signal::CAPS_LOCK;
            break;
        case GLFW_KEY_SCROLL_LOCK :
            signal = Signal::SCROLL_LOCK;
            break;
        case GLFW_KEY_NUM_LOCK :
            signal = Signal::NUM_LOCK;
            break;
        case GLFW_KEY_PRINT_SCREEN :
            signal = Signal::PRINT_SCREEN;
            break;
        case GLFW_KEY_PAUSE :
            signal = Signal::PAUSE;
            break;
        case GLFW_KEY_F1 :
            signal = Signal::F1;
            break;
        case GLFW_KEY_F2 :
            signal = Signal::F2;
            break;
        case GLFW_KEY_F3 :
            signal = Signal::F3;
            break;
        case GLFW_KEY_F4 :
            signal = Signal::F4;
            break;
        case GLFW_KEY_F5 :
            signal = Signal::F5;
            break;
        case GLFW_KEY_F6 :
            signal = Signal::F6;
            break;
        case GLFW_KEY_F7 :
            signal = Signal::F7;
            break;
        case GLFW_KEY_F8 :
            signal = Signal::F8;
            break;
        case GLFW_KEY_F9 :
            signal = Signal::F9;
            break;
        case GLFW_KEY_F10 :
            signal = Signal::F10;
            break;
        case GLFW_KEY_F11 :
            signal = Signal::F11;
            break;
        case GLFW_KEY_F12 :
            signal = Signal::F12;
            break;
        case GLFW_KEY_F13 :
            signal = Signal::F13;
            break;
        case GLFW_KEY_F14 :
            signal = Signal::F14;
            break;
        case GLFW_KEY_F15 :
            signal = Signal::F15;
            break;
        case GLFW_KEY_F16 :
            signal = Signal::F16;
            break;
        case GLFW_KEY_F17 :
            signal = Signal::F17;
            break;
        case GLFW_KEY_F18 :
            signal = Signal::F18;
            break;
        case GLFW_KEY_F19 :
            signal = Signal::F19;
            break;
        case GLFW_KEY_F20 :
            signal = Signal::F20;
            break;
        case GLFW_KEY_F21 :
            signal = Signal::F21;
            break;
        case GLFW_KEY_F22 :
            signal = Signal::F22;
            break;
        case GLFW_KEY_F23 :
            signal = Signal::F23;
            break;
        case GLFW_KEY_F24 :
            signal = Signal::F24;
            break;
        case GLFW_KEY_F25 :
            signal = Signal::F25;
            break;
        case GLFW_KEY_KP_0 :
            signal = Signal::KP0;
            break;
        case GLFW_KEY_KP_1 :
            signal = Signal::KP1;
            break;
        case GLFW_KEY_KP_2 :
            signal = Signal::KP2;
            break;
        case GLFW_KEY_KP_3 :
            signal = Signal::KP3;
            break;
        case GLFW_KEY_KP_4 :
            signal = Signal::KP4;
            break;
        case GLFW_KEY_KP_5 :
            signal = Signal::KP5;
            break;
        case GLFW_KEY_KP_6 :
            signal = Signal::KP6;
            break;
        case GLFW_KEY_KP_7 :
            signal = Signal::KP7;
            break;
        case GLFW_KEY_KP_8 :
            signal = Signal::KP8;
            break;
        case GLFW_KEY_KP_9 :
            signal = Signal::KP9;
            break;
        case GLFW_KEY_KP_DECIMAL :
            signal = Signal::KP_DECIMAL;
            break;
        case GLFW_KEY_KP_DIVIDE :
            signal = Signal::KP_DIVIDE;
            break;
        case GLFW_KEY_KP_MULTIPLY :
            signal = Signal::KP_MULTIPLY;
            break;
        case GLFW_KEY_KP_SUBTRACT :
            signal = Signal::KP_SUBTRACT;
            break;
        case GLFW_KEY_KP_ADD :
            signal = Signal::KP_ADD;
            break;
        case GLFW_KEY_KP_ENTER :
            signal = Signal::KP_ENTER;
            break;
        case GLFW_KEY_KP_EQUAL :
            signal = Signal::KP_EQUAL;
            break;
        case GLFW_KEY_LEFT_SHIFT :
            signal = Signal::LEFT_SHIFT;
            break;
        case GLFW_KEY_LEFT_CONTROL :
            signal = Signal::LEFT_CTRL;
            break;
        case GLFW_KEY_LEFT_ALT :
            signal = Signal::LEFT_ALT;
            break;
        case GLFW_KEY_LEFT_SUPER :
            signal = Signal::LEFT_SUPER;
            break;
        case GLFW_KEY_RIGHT_SHIFT :
            signal = Signal::RIGHT_SHIFT;
            break;
        case GLFW_KEY_RIGHT_CONTROL :
            signal = Signal::RIGHT_CTRL;
            break;
        case GLFW_KEY_RIGHT_ALT :
            signal = Signal::RIGHT_ALT;
            break;
        case GLFW_KEY_RIGHT_SUPER :
            signal = Signal::RIGHT_SUPER;
            break;
        case GLFW_KEY_MENU :
            signal = Signal::MENU;
            break;
        
        default:
            signal = Signal::UNKNOWN;
            break;
    }

    return signal;
}


Signal GlfwMouseButtonCode(int glfwKey) {

    Signal signal;
    switch (glfwKey)
    {
        case GLFW_MOUSE_BUTTON_1 :
            signal = Signal::MOUSE_1;
            break;
        case GLFW_MOUSE_BUTTON_2 :
            signal = Signal::MOUSE_2;
            break;
        case GLFW_MOUSE_BUTTON_3 :
            signal = Signal::MOUSE_3;
            break;
        case GLFW_MOUSE_BUTTON_4 :
            signal = Signal::MOUSE_4;
            break;
        case GLFW_MOUSE_BUTTON_5 :
            signal = Signal::MOUSE_5;
            break;
        case GLFW_MOUSE_BUTTON_6 :
            signal = Signal::MOUSE_6;
            break;
        case GLFW_MOUSE_BUTTON_7 :
            signal = Signal::MOUSE_7;
            break;
        case GLFW_MOUSE_BUTTON_8 :
            signal = Signal::MOUSE_8;
            break;
        
        default:
            signal = Signal::UNKNOWN;
            break;
    }

    return signal;
}


Signal GlfwGamepadButtonCode(int glfwKey) {

    Signal signal;
    switch (glfwKey)
    {
        case GLFW_GAMEPAD_BUTTON_A :
            signal = Signal::GAMEPAD_A;
            break;
        case GLFW_GAMEPAD_BUTTON_B :
            signal = Signal::GAMEPAD_B;
            break;
        case GLFW_GAMEPAD_BUTTON_X :
            signal = Signal::GAMEPAD_X;
            break;
        case GLFW_GAMEPAD_BUTTON_Y :
            signal = Signal::GAMEPAD_Y;
            break;
        case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER :
            signal = Signal::GAMEPAD_LEFT_BUMPER;
            break;
        case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER :
            signal = Signal::GAMEPAD_RIGHT_BUMPER;
            break;
        case GLFW_GAMEPAD_BUTTON_BACK :
            signal = Signal::GAMEPAD_BACK;
            break;
        case GLFW_GAMEPAD_BUTTON_START :
            signal = Signal::GAMEPAD_GUIDE;
            break;
        case GLFW_GAMEPAD_BUTTON_LEFT_THUMB :
            signal = Signal::GAMEPAD_LEFT_THUMB;
            break;
        case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB :
            signal = Signal::GAMEPAD_RIGHT_THUMB;
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_UP :
            signal = Signal::GAMEPAD_DPAD_UP;
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT :
            signal = Signal::GAMEPAD_DPAD_RIGHT;
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_DOWN :
            signal = Signal::GAMEPAD_DPAD_DOWN;
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_LEFT :
            signal = Signal::GAMEPAD_DPAD_LEFT;
            break;
        
        default:
            signal = Signal::UNKNOWN;
            break;
    }

    return signal;
}


Signal GlfwGamepadAxesCode(int glfwKey) {

    Signal signal;
    switch (glfwKey)
    {
        case GLFW_GAMEPAD_AXIS_LEFT_X :
            signal = Signal::GAMEPAD_AXIS_LEFT_X;
            break;
        case GLFW_GAMEPAD_AXIS_LEFT_Y :
            signal = Signal::GAMEPAD_AXIS_LEFT_Y;
            break;
        case GLFW_GAMEPAD_AXIS_RIGHT_X :
            signal = Signal::GAMEPAD_AXIS_RIGHT_X;
            break;
        case GLFW_GAMEPAD_AXIS_RIGHT_Y :
            signal = Signal::GAMEPAD_AXIS_RIGHT_Y;
            break;
        case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER :
            signal = Signal::GAMEPAD_AXIS_LEFT_TRIGGER;
            break;
        case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER :
            signal = Signal::GAMEPAD_AXIS_RIGHT_TRIGGER;
            break;
        
        default:
            signal = Signal::UNKNOWN;
            break;
    }

    return signal;
}

    
} // Atuin
