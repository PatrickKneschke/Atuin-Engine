
#pragma once


#include "KeyCodes.h"

#include "GLFW/glfw3.h"


namespace Atuin {


Key GlfwKeyCode(int glfwKey) {

    Key key;
    switch (glfwKey)
    {
        case GLFW_KEY_UNKNOWN :
            key = Key::UNKNOWN;
            break;
        case GLFW_KEY_SPACE :
            key = Key::SPACE;
            break;
        case GLFW_KEY_APOSTROPHE :
            key = Key::APOSTROPHE;
            break;
        case GLFW_KEY_COMMA :
            key = Key::COMMA;
            break;
        case GLFW_KEY_MINUS :
            key = Key::MINUS;
            break;
        case GLFW_KEY_PERIOD :
            key = Key::PERIOD;
            break;
        case GLFW_KEY_SLASH :
            key = Key::SLASH;
            break;
        case GLFW_KEY_0 :
            key = Key::NUM0;
            break;
        case GLFW_KEY_1 :
            key = Key::NUM1;
            break;
        case GLFW_KEY_2 :
            key = Key::NUM2;
            break;
        case GLFW_KEY_3 :
            key = Key::NUM3;
            break;
        case GLFW_KEY_4 :
            key = Key::NUM4;
            break;
        case GLFW_KEY_5 :
            key = Key::NUM5;
            break;
        case GLFW_KEY_6 :
            key = Key::NUM6;
            break;
        case GLFW_KEY_7 :
            key = Key::NUM7;
            break;
        case GLFW_KEY_8 :
            key = Key::NUM8;
            break;
        case GLFW_KEY_9 :
            key = Key::NUM9;
            break;
        case GLFW_KEY_SEMICOLON :
            key = Key::SEMICOLON;
            break;
        case GLFW_KEY_EQUAL :
            key = Key::EQUAL;
            break;
        case GLFW_KEY_A :
            key = Key::A;
            break;
        case GLFW_KEY_B :
            key = Key::B;
            break;
        case GLFW_KEY_C :
            key = Key::C;
            break;
        case GLFW_KEY_D :
            key = Key::D;
            break;
        case GLFW_KEY_E :
            key = Key::E;
            break;
        case GLFW_KEY_F :
            key = Key::F;
            break;
        case GLFW_KEY_G :
            key = Key::G;
            break;
        case GLFW_KEY_H :
            key = Key::H;
            break;
        case GLFW_KEY_I :
            key = Key::I;
            break;
        case GLFW_KEY_J :
            key = Key::J;
            break;
        case GLFW_KEY_K :
            key = Key::K;
            break;
        case GLFW_KEY_L :
            key = Key::L;
            break;
        case GLFW_KEY_M :
            key = Key::M;
            break;
        case GLFW_KEY_N :
            key = Key::N;
            break;
        case GLFW_KEY_O :
            key = Key::O;
            break;
        case GLFW_KEY_P :
            key = Key::P;
            break;
        case GLFW_KEY_Q :
            key = Key::Q;
            break;
        case GLFW_KEY_R :
            key = Key::R;
            break;
        case GLFW_KEY_S :
            key = Key::S;
            break;
        case GLFW_KEY_T :
            key = Key::T;
            break;
        case GLFW_KEY_U :
            key = Key::U;
            break;
        case GLFW_KEY_V :
            key = Key::V;
            break;
        case GLFW_KEY_W :
            key = Key::W;
            break;
        case GLFW_KEY_X :
            key = Key::X;
            break;
        case GLFW_KEY_Y :
            key = Key::Y;
            break;
        case GLFW_KEY_Z :
            key = Key::Z;
            break;
        case GLFW_KEY_LEFT_BRACKET :
            key = Key::LEFT_BRACKET;
            break;
        case GLFW_KEY_BACKSLASH :
            key = Key::BACKSLASH;
            break;
        case GLFW_KEY_RIGHT_BRACKET :
            key = Key::RIGHT_BRACKET;
            break;
        case GLFW_KEY_GRAVE_ACCENT :
            key = Key::GRAVE_ACCENT;
            break;
        case GLFW_KEY_ESCAPE :
            key = Key::ESCAPE;
            break;
        case GLFW_KEY_ENTER :
            key = Key::ENTER;
            break;
        case GLFW_KEY_TAB :
            key = Key::TAB;
            break;
        case GLFW_KEY_BACKSPACE :
            key = Key::BACKSPACE;
            break;
        case GLFW_KEY_INSERT :
            key = Key::INSERT;
            break;
        case GLFW_KEY_DELETE :
            key = Key::DELETE;
            break;
        case GLFW_KEY_RIGHT :
            key = Key::RIGHT;
            break;
        case GLFW_KEY_LEFT :
            key = Key::LEFT;
            break;
        case GLFW_KEY_DOWN :
            key = Key::DOWN;
            break;
        case GLFW_KEY_UP :
            key = Key::UP;
            break;
        case GLFW_KEY_PAGE_UP :
            key = Key::PAGE_UP;
            break;
        case GLFW_KEY_PAGE_DOWN :
            key = Key::PAGE_DOWN;
            break;
        case GLFW_KEY_HOME :
            key = Key::HOME;
            break;
        case GLFW_KEY_END :
            key = Key::END;
            break;
        case GLFW_KEY_CAPS_LOCK :
            key = Key::CAPS_LOCK;
            break;
        case GLFW_KEY_SCROLL_LOCK :
            key = Key::SCROLL_LOCK;
            break;
        case GLFW_KEY_NUM_LOCK :
            key = Key::NUM_LOCK;
            break;
        case GLFW_KEY_PRINT_SCREEN :
            key = Key::PRINT_SCREEN;
            break;
        case GLFW_KEY_PAUSE :
            key = Key::PAUSE;
            break;
        case GLFW_KEY_F1 :
            key = Key::F1;
            break;
        case GLFW_KEY_F2 :
            key = Key::F2;
            break;
        case GLFW_KEY_F3 :
            key = Key::F3;
            break;
        case GLFW_KEY_F4 :
            key = Key::F4;
            break;
        case GLFW_KEY_F5 :
            key = Key::F5;
            break;
        case GLFW_KEY_F6 :
            key = Key::F6;
            break;
        case GLFW_KEY_F7 :
            key = Key::F7;
            break;
        case GLFW_KEY_F8 :
            key = Key::F8;
            break;
        case GLFW_KEY_F9 :
            key = Key::F9;
            break;
        case GLFW_KEY_F10 :
            key = Key::F10;
            break;
        case GLFW_KEY_F11 :
            key = Key::F11;
            break;
        case GLFW_KEY_F12 :
            key = Key::F12;
            break;
        case GLFW_KEY_F13 :
            key = Key::F13;
            break;
        case GLFW_KEY_F14 :
            key = Key::F14;
            break;
        case GLFW_KEY_F15 :
            key = Key::F15;
            break;
        case GLFW_KEY_F16 :
            key = Key::F16;
            break;
        case GLFW_KEY_F17 :
            key = Key::F17;
            break;
        case GLFW_KEY_F18 :
            key = Key::F18;
            break;
        case GLFW_KEY_F19 :
            key = Key::F19;
            break;
        case GLFW_KEY_F20 :
            key = Key::F20;
            break;
        case GLFW_KEY_F21 :
            key = Key::F21;
            break;
        case GLFW_KEY_F22 :
            key = Key::F22;
            break;
        case GLFW_KEY_F23 :
            key = Key::F23;
            break;
        case GLFW_KEY_F24 :
            key = Key::F24;
            break;
        case GLFW_KEY_F25 :
            key = Key::F25;
            break;
        case GLFW_KEY_KP_0 :
            key = Key::KP0;
            break;
        case GLFW_KEY_KP_1 :
            key = Key::KP1;
            break;
        case GLFW_KEY_KP_2 :
            key = Key::KP2;
            break;
        case GLFW_KEY_KP_3 :
            key = Key::KP3;
            break;
        case GLFW_KEY_KP_4 :
            key = Key::KP4;
            break;
        case GLFW_KEY_KP_5 :
            key = Key::KP5;
            break;
        case GLFW_KEY_KP_6 :
            key = Key::KP6;
            break;
        case GLFW_KEY_KP_7 :
            key = Key::KP7;
            break;
        case GLFW_KEY_KP_8 :
            key = Key::KP8;
            break;
        case GLFW_KEY_KP_9 :
            key = Key::KP9;
            break;
        case GLFW_KEY_KP_DECIMAL :
            key = Key::KP_DECIMAL;
            break;
        case GLFW_KEY_KP_DIVIDE :
            key = Key::KP_DIVIDE;
            break;
        case GLFW_KEY_KP_MULTIPLY :
            key = Key::KP_MULTIPLY;
            break;
        case GLFW_KEY_KP_SUBTRACT :
            key = Key::KP_SUBTRACT;
            break;
        case GLFW_KEY_KP_ADD :
            key = Key::KP_ADD;
            break;
        case GLFW_KEY_KP_ENTER :
            key = Key::KP_ENTER;
            break;
        case GLFW_KEY_KP_EQUAL :
            key = Key::KP_EQUAL;
            break;
        case GLFW_KEY_LEFT_SHIFT :
            key = Key::LEFT_SHIFT;
            break;
        case GLFW_KEY_LEFT_CONTROL :
            key = Key::LEFT_CTRL;
            break;
        case GLFW_KEY_LEFT_ALT :
            key = Key::LEFT_ALT;
            break;
        case GLFW_KEY_LEFT_SUPER :
            key = Key::LEFT_SUPER;
            break;
        case GLFW_KEY_RIGHT_SHIFT :
            key = Key::RIGHT_SHIFT;
            break;
        case GLFW_KEY_RIGHT_CONTROL :
            key = Key::RIGHT_CTRL;
            break;
        case GLFW_KEY_RIGHT_ALT :
            key = Key::RIGHT_ALT;
            break;
        case GLFW_KEY_RIGHT_SUPER :
            key = Key::RIGHT_SUPER;
            break;
        case GLFW_KEY_MENU :
            key = Key::MENU;
            break;
        
        default:
            break;
    }

    return key;
}


Key GlfwMouseButtonCode(int glfwKey) {

    Key key;
    switch (glfwKey)
    {
        case GLFW_MOUSE_BUTTON_1 :
            key = Key::MOUSE_1;
            break;
        case GLFW_MOUSE_BUTTON_2 :
            key = Key::MOUSE_2;
            break;
        case GLFW_MOUSE_BUTTON_3 :
            key = Key::MOUSE_3;
            break;
        case GLFW_MOUSE_BUTTON_4 :
            key = Key::MOUSE_4;
            break;
        case GLFW_MOUSE_BUTTON_5 :
            key = Key::MOUSE_5;
            break;
        case GLFW_MOUSE_BUTTON_6 :
            key = Key::MOUSE_6;
            break;
        case GLFW_MOUSE_BUTTON_7 :
            key = Key::MOUSE_7;
            break;
        case GLFW_MOUSE_BUTTON_8 :
            key = Key::MOUSE_8;
            break;
        
        default:
            break;
    }

    return key;
}


Key GlfwGamepadButtonCode(int glfwKey) {

    Key key;
    switch (glfwKey)
    {
        case GLFW_GAMEPAD_BUTTON_A :
            key = Key::GAMEPAD_A;
            break;
        case GLFW_GAMEPAD_BUTTON_B :
            key = Key::GAMEPAD_B;
            break;
        case GLFW_GAMEPAD_BUTTON_X :
            key = Key::GAMEPAD_X;
            break;
        case GLFW_GAMEPAD_BUTTON_Y :
            key = Key::GAMEPAD_Y;
            break;
        case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER :
            key = Key::GAMEPAD_LEFT_BUMPER;
            break;
        case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER :
            key = Key::GAMEPAD_RIGHT_BUMPER;
            break;
        case GLFW_GAMEPAD_BUTTON_BACK :
            key = Key::GAMEPAD_BACK;
            break;
        case GLFW_GAMEPAD_BUTTON_START :
            key = Key::GAMEPAD_GUIDE;
            break;
        case GLFW_GAMEPAD_BUTTON_LEFT_THUMB :
            key = Key::GAMEPAD_LEFT_THUMB;
            break;
        case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB :
            key = Key::GAMEPAD_RIGHT_THUMB;
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_UP :
            key = Key::GAMEPAD_DPAD_UP;
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT :
            key = Key::GAMEPAD_DPAD_RIGHT;
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_DOWN :
            key = Key::GAMEPAD_DPAD_DOWN;
            break;
        case GLFW_GAMEPAD_BUTTON_DPAD_LEFT :
            key = Key::GAMEPAD_DPAD_LEFT;
            break;
        
        default:
            break;
    }

    return key;
}


Key GlfwGamepadAxesCode(int glfwKey) {

    Key key;
    switch (glfwKey)
    {
        case GLFW_GAMEPAD_AXIS_LEFT_X :
            key = Key::GAMEPAD_AXIS_LEFT_X;
            break;
        case GLFW_GAMEPAD_AXIS_LEFT_Y :
            key = Key::GAMEPAD_AXIS_LEFT_Y;
            break;
        case GLFW_GAMEPAD_AXIS_RIGHT_X :
            key = Key::GAMEPAD_AXIS_RIGHT_X;
            break;
        case GLFW_GAMEPAD_AXIS_RIGHT_Y :
            key = Key::GAMEPAD_AXIS_RIGHT_Y;
            break;
        case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER :
            key = Key::GAMEPAD_AXIS_LEFT_TRIGGER;
            break;
        case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER :
            key = Key::GAMEPAD_AXIS_RIGHT_TRIGGER;
            break;
        
        default:
            break;
    }

    return key;
}

    
} // Atuin
