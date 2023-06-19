
#include "InputSignals.h"


namespace Atuin {


const std::string ToString(Signal signal) {

    std::string signalName;
    switch (signal)
    {
        case Signal::UNKNOWN :
            signalName = "UNKNOWN";
            break;
        case Signal::SPACE :
            signalName = "SPACE";
            break;
        case Signal::APOSTROPHE :
            signalName = "APOSTROPHE";
            break;
        case Signal::COMMA :
            signalName = "COMMA";
            break;
        case Signal::MINUS :
            signalName = "MINUS";
            break;
        case Signal::PERIOD :
            signalName = "PERIOD";
            break;
        case Signal::SLASH :
            signalName = "SLASH";
            break;
        case Signal::NUM0 :
            signalName = "NUM0";
            break;
        case Signal::NUM1 :
            signalName = "NUM1";
            break;
        case Signal::NUM2 :
            signalName = "NUM2";
            break;
        case Signal::NUM3 :
            signalName = "NUM3";
            break;
        case Signal::NUM4 :
            signalName = "NUM4";
            break;
        case Signal::NUM5 :
            signalName = "NUM5";
            break;
        case Signal::NUM6 :
            signalName = "NUM6";
            break;
        case Signal::NUM7 :
            signalName = "NUM7";
            break;
        case Signal::NUM8 :
            signalName = "NUM8";
            break;
        case Signal::NUM9 :
            signalName = "NUM9";
            break;
        case Signal::SEMICOLON :
            signalName = "SEMICOLON";
            break;
        case Signal::EQUAL :
            signalName = "EQUAL";
            break;
        case Signal::A :
            signalName = "A";
            break;
        case Signal::B :
            signalName = "B";
            break;
        case Signal::C :
            signalName = "C";
            break;
        case Signal::D :
            signalName = "D";
            break;
        case Signal::E :
            signalName = "E";
            break;
        case Signal::F :
            signalName = "F";
            break;
        case Signal::G :
            signalName = "G";
            break;
        case Signal::H :
            signalName = "H";
            break;
        case Signal::I :
            signalName = "I";
            break;
        case Signal::J :
            signalName = "J";
            break;
        case Signal::K :
            signalName = "K";
            break;
        case Signal::L :
            signalName = "L";
            break;
        case Signal::M :
            signalName = "M";
            break;
        case Signal::N :
            signalName = "N";
            break;
        case Signal::O :
            signalName = "O";
            break;
        case Signal::P :
            signalName = "P";
            break;
        case Signal::Q :
            signalName = "Q";
            break;
        case Signal::R :
            signalName = "R";
            break;
        case Signal::S :
            signalName = "S";
            break;
        case Signal::T :
            signalName = "T";
            break;
        case Signal::U :
            signalName = "U";
            break;
        case Signal::V :
            signalName = "V";
            break;
        case Signal::W :
            signalName = "W";
            break;
        case Signal::X :
            signalName = "X";
            break;
        case Signal::Y :
            signalName = "Y";
            break;
        case Signal::Z :
            signalName = "Z";
            break;
        case Signal::LEFT_BRACKET :
            signalName = "LEFT_BRACKET";
            break;
        case Signal::BACKSLASH :
            signalName = "BACKSLASH";
            break;
        case Signal::RIGHT_BRACKET :
            signalName = "RIGHT_BRACKET";
            break;
        case Signal::GRAVE_ACCENT :
            signalName = "GRAVE_ACCENT";
            break;
        case Signal::ESCAPE :
            signalName = "ESCAPE";
            break;
        case Signal::ENTER :
            signalName = "ENTER";
            break;
        case Signal::TAB :
            signalName = "TAB";
            break;
        case Signal::BACKSPACE :
            signalName = "BACKSPACE";
            break;
        case Signal::INSERT :
            signalName = "INSERT";
            break;
        case Signal::DELETE :
            signalName = "DELETE";
            break;
        case Signal::RIGHT :
            signalName = "RIGHT";
            break;
        case Signal::LEFT :
            signalName = "LEFT";
            break;
        case Signal::DOWN :
            signalName = "DOWN";
            break;
        case Signal::UP :
            signalName = "UP";
            break;
        case Signal::PAGE_UP :
            signalName = "PAGE_UP";
            break;
        case Signal::PAGE_DOWN :
            signalName = "PAGE_DOWN";
            break;
        case Signal::HOME :
            signalName = "HOME";
            break;
        case Signal::END :
            signalName = "END";
            break;
        case Signal::CAPS_LOCK :
            signalName = "CAPS_LOCK";
            break;
        case Signal::SCROLL_LOCK :
            signalName = "SCROLL_LOCK";
            break;
        case Signal::NUM_LOCK :
            signalName = "NUM_LOCK";
            break;
        case Signal::PRINT_SCREEN :
            signalName = "PRINT_SCREEN";
            break;
        case Signal::PAUSE :
            signalName = "PAUSE";
            break;
        case Signal::F1 :
            signalName = "F1";
            break;
        case Signal::F2 :
            signalName = "F2";
            break;
        case Signal::F3 :
            signalName = "F3";
            break;
        case Signal::F4 :
            signalName = "F4";
            break;
        case Signal::F5 :
            signalName = "F5";
            break;
        case Signal::F6 :
            signalName = "F6";
            break;
        case Signal::F7 :
            signalName = "F7";
            break;
        case Signal::F8 :
            signalName = "F8";
            break;
        case Signal::F9 :
            signalName = "F9";
            break;
        case Signal::F10 :
            signalName = "F10";
            break;
        case Signal::F11 :
            signalName = "F11";
            break;
        case Signal::F12 :
            signalName = "F12";
            break;
        case Signal::F13 :
            signalName = "F13";
            break;
        case Signal::F14 :
            signalName = "F14";
            break;
        case Signal::F15 :
            signalName = "F15";
            break;
        case Signal::F16 :
            signalName = "F16";
            break;
        case Signal::F17 :
            signalName = "F17";
            break;
        case Signal::F18 :
            signalName = "F18";
            break;
        case Signal::F19 :
            signalName = "F19";
            break;
        case Signal::F20 :
            signalName = "F20";
            break;
        case Signal::F21 :
            signalName = "F21";
            break;
        case Signal::F22 :
            signalName = "F22";
            break;
        case Signal::F23 :
            signalName = "F23";
            break;
        case Signal::F24 :
            signalName = "F24";
            break;
        case Signal::F25 :
            signalName = "F25";
            break;
        case Signal::KP0 :
            signalName = "KP0";
            break;
        case Signal::KP1 :
            signalName = "KP1";
            break;
        case Signal::KP2 :
            signalName = "KP2";
            break;
        case Signal::KP3 :
            signalName = "KP3";
            break;
        case Signal::KP4 :
            signalName = "KP4";
            break;
        case Signal::KP5 :
            signalName = "KP5";
            break;
        case Signal::KP6 :
            signalName = "KP6";
            break;
        case Signal::KP7 :
            signalName = "KP7";
            break;
        case Signal::KP8 :
            signalName = "KP8";
            break;
        case Signal::KP9 :
            signalName = "KP9";
            break;
        case Signal::KP_DECIMAL :
            signalName = "KP_DECIMAL";
            break;
        case Signal::KP_DIVIDE :
            signalName = "KP_DIVIDE";
            break;
        case Signal::KP_MULTIPLY :
            signalName = "KP_MULTIPLY";
            break;
        case Signal::KP_SUBTRACT :
            signalName = "KP_SUBTRACT";
            break;
        case Signal::KP_ADD :
            signalName = "KP_ADD";
            break;
        case Signal::KP_ENTER :
            signalName = "KP_ENTER";
            break;
        case Signal::KP_EQUAL :
            signalName = "KP_EQUAL";
            break;
        case Signal::LEFT_SHIFT :
            signalName = "LEFT_SHIFT";
            break;
        case Signal::LEFT_CTRL :
            signalName = "LEFT_CTRL";
            break;
        case Signal::LEFT_ALT :
            signalName = "LEFT_ALT";
            break;
        case Signal::LEFT_SUPER :
            signalName = "LEFT_SUPER";
            break;
        case Signal::RIGHT_SHIFT :
            signalName = "RIGHT_SHIFT";
            break;
        case Signal::RIGHT_CTRL :
            signalName = "RIGHT_CTRL";
            break;
        case Signal::RIGHT_ALT :
            signalName = "RIGHT_ALT";
            break;
        case Signal::RIGHT_SUPER :
            signalName = "RIGHT_SUPER";
            break;
        case Signal::MENU :
            signalName = "MENU";
            break;
        case Signal::MOUSE_1 :
            signalName = "MOUSE_LEFT";
            break;
        case Signal::MOUSE_2 :
            signalName = "MOUSE_RIGHT";
            break;
        case Signal::MOUSE_3 :
            signalName = "MOUSE_MIDDLE";
            break;
        case Signal::MOUSE_4 :
            signalName = "MOUSE_4";
            break;
        case Signal::MOUSE_5 :
            signalName = "MOUSE_5";
            break;
        case Signal::MOUSE_6 :
            signalName = "MOUSE_6";
            break;
        case Signal::MOUSE_7 :
            signalName = "MOUSE_7";
            break;
        case Signal::MOUSE_8 :
            signalName = "MOUSE_8";
            break;
        case Signal::MOUSE_MOVE_X :
            signalName = "MOUSE_MOVE_X";
            break;
        case Signal::MOUSE_MOVE_Y :
            signalName = "MOUSE_MOVE_Y";
            break;
        case Signal::MOUSE_SCROLL_UP :
            signalName = "MOUSE_SCROLL_UP";
            break;
        case Signal::MOUSE_SCROLL_DOWN :
            signalName = "MOUSE_SCROLL_DOWN";
            break;
        case Signal::MOUSE_SCROLL_RIGHT :
            signalName = "MOUSE_SCROLL_RIGHT";
            break;
        case Signal::MOUSE_SCROLL_LEFT :
            signalName = "MOUSE_SCROLL_LEFT";
            break;
        case Signal::GAMEPAD_A :
            signalName = "GAMEPAD_A";
            break;
        case Signal::GAMEPAD_B :
            signalName = "GAMEPAD_B";
            break;
        case Signal::GAMEPAD_X :
            signalName = "GAMEPAD_X";
            break;
        case Signal::GAMEPAD_Y :
            signalName = "GAMEPAD_Y";
            break;
        case Signal::GAMEPAD_LEFT_BUMPER :
            signalName = "GAMEPAD_LEFT_BUMPER";
            break;
        case Signal::GAMEPAD_RIGHT_BUMPER :
            signalName = "GAMEPAD_RIGHT_BUMPER";
            break;
        case Signal::GAMEPAD_BACK :
            signalName = "GAMEPAD_BACK";
            break;
        case Signal::GAMEPAD_START :
            signalName = "GAMEPAD_START";
            break;
        case Signal::GAMEPAD_GUIDE :
            signalName = "GAMEPAD_GUIDE";
            break;
        case Signal::GAMEPAD_LEFT_THUMB :
            signalName = "GAMEPAD_LEFT_THUMB";
            break;
        case Signal::GAMEPAD_RIGHT_THUMB :
            signalName = "GAMEPAD_RIGHT_THUMB";
            break;
        case Signal::GAMEPAD_DPAD_UP :
            signalName = "GAMEPAD_DPAD_UP";
            break;
        case Signal::GAMEPAD_DPAD_RIGHT :
            signalName = "GAMEPAD_DPAD_RIGHT";
            break;
        case Signal::GAMEPAD_DPAD_DOWN :
            signalName = "GAMEPAD_DPAD_DOWN";
            break;
        case Signal::GAMEPAD_DPAD_LEFT :
            signalName = "GAMEPAD_DPAD_LEFT";
            break;
        case Signal::GAMEPAD_AXIS_LEFT_X :
            signalName = "GAMEPAD_AXIS_LEFT_X";
            break;
        case Signal::GAMEPAD_AXIS_LEFT_Y :
            signalName = "GAMEPAD_AXIS_LEFT_Y";
            break;
        case Signal::GAMEPAD_AXIS_RIGHT_X :
            signalName = "GAMEPAD_AXIS_RIGHT_X";
            break;
        case Signal::GAMEPAD_AXIS_RIGHT_Y :
            signalName = "GAMEPAD_AXIS_RIGHT_Y";
            break;
        case Signal::GAMEPAD_AXIS_LEFT_TRIGGER :
            signalName = "GAMEPAD_AXIS_LEFT_TRIGGER";
            break;
        case Signal::GAMEPAD_AXIS_RIGHT_TRIGGER :
            signalName = "GAMEPAD_AXIS_RIGHT_TRIGGER";
            break;
        
        default:
            signalName = "Unhandled Signal";
            break;
    }

    return signalName;
}


Signal StringToSignal(std::string_view signalName) {

    Signal signal;
    U64 signalID = SID(signalName.data());
    switch(signalID) 
    {
        case SID("SPACE") :
            signal = Signal::SPACE;
            break;
        case SID("APOSTROPHE") :
            signal = Signal::APOSTROPHE;
            break;
        case SID("COMMA") :
            signal = Signal::COMMA;
            break;
        case SID("MINUS") :
            signal = Signal::MINUS;
            break;
        case SID("PERIOD") :
            signal = Signal::PERIOD;
            break;
        case SID("SLASH") :
            signal = Signal::SLASH;
            break;
        case SID("NUM0") :
            signal = Signal::NUM0;
            break;
        case SID("NUM1") :
            signal = Signal::NUM1;
            break;
        case SID("NUM2") :
            signal = Signal::NUM2;
            break;
        case SID("NUM3") :
            signal = Signal::NUM3;
            break;
        case SID("NUM4") :
            signal = Signal::NUM4;
            break;
        case SID("NUM5") :
            signal = Signal::NUM5;
            break;
        case SID("NUM6") :
            signal = Signal::NUM6;
            break;
        case SID("NUM7") :
            signal = Signal::NUM7;
            break;
        case SID("NUM8") :
            signal = Signal::NUM8;
            break;
        case SID("NUM9") :
            signal = Signal::NUM9;
            break;
        case SID("SEMICOLON") :
            signal = Signal::SEMICOLON;
            break;
        case SID("EQUAL") :
            signal = Signal::EQUAL;
            break;
        case SID("A") :
            signal = Signal::A;
            break;
        case SID("B") :
            signal = Signal::B;
            break;
        case SID("C") :
            signal = Signal::C;
            break;
        case SID("D") :
            signal = Signal::D;
            break;
        case SID("E") :
            signal = Signal::E;
            break;
        case SID("F") :
            signal = Signal::F;
            break;
        case SID("G") :
            signal = Signal::G;
            break;
        case SID("H") :
            signal = Signal::H;
            break;
        case SID("I") :
            signal = Signal::I;
            break;
        case SID("J") :
            signal = Signal::J;
            break;
        case SID("K") :
            signal = Signal::K;
            break;
        case SID("L") :
            signal = Signal::L;
            break;
        case SID("M") :
            signal = Signal::M;
            break;
        case SID("N") :
            signal = Signal::N;
            break;
        case SID("O") :
            signal = Signal::O;
            break;
        case SID("P") :
            signal = Signal::P;
            break;
        case SID("Q") :
            signal = Signal::Q;
            break;
        case SID("R") :
            signal = Signal::R;
            break;
        case SID("S") :
            signal = Signal::S;
            break;
        case SID("T") :
            signal = Signal::T;
            break;
        case SID("U") :
            signal = Signal::U;
            break;
        case SID("V") :
            signal = Signal::V;
            break;
        case SID("W") :
            signal = Signal::W;
            break;
        case SID("X") :
            signal = Signal::X;
            break;
        case SID("Y") :
            signal = Signal::Y;
            break;
        case SID("Z") :
            signal = Signal::Z;
            break;
        case SID("LEFT_BRACKET") :
            signal = Signal::LEFT_BRACKET;
            break;
        case SID("BACKSLASH") :
            signal = Signal::BACKSLASH;
            break;
        case SID("RIGHT_BRACKET") :
            signal = Signal::RIGHT_BRACKET;
            break;
        case SID("GRAVE_ACCENT") :
            signal = Signal::GRAVE_ACCENT;
            break;
        case SID("ESCAPE") :
            signal = Signal::ESCAPE;
            break;
        case SID("ENTER") :
            signal = Signal::ENTER;
            break;
        case SID("TAB") :
            signal = Signal::TAB;
            break;
        case SID("BACKSPACE") :
            signal = Signal::BACKSPACE;
            break;
        case SID("INSERT") :
            signal = Signal::INSERT;
            break;
        case SID("DELETE") :
            signal = Signal::DELETE;
            break;
        case SID("RIGHT") :
            signal = Signal::RIGHT;
            break;
        case SID("LEFT") :
            signal = Signal::LEFT;
            break;
        case SID("DOWN") :
            signal = Signal::DOWN;
            break;
        case SID("UP") :
            signal = Signal::UP;
            break;
        case SID("PAGE_UP") :
            signal = Signal::PAGE_UP;
            break;
        case SID("PAGE_DOWN") :
            signal = Signal::PAGE_DOWN;
            break;
        case SID("HOME") :
            signal = Signal::HOME;
            break;
        case SID("END") :
            signal = Signal::END;
            break;
        case SID("CAPS_LOCK") :
            signal = Signal::CAPS_LOCK;
            break;
        case SID("SCROLL_LOCK") :
            signal = Signal::SCROLL_LOCK;
            break;
        case SID("NUM_LOCK") :
            signal = Signal::NUM_LOCK;
            break;
        case SID("PRINT_SCREEN") :
            signal = Signal::PRINT_SCREEN;
            break;
        case SID("PAUSE") :
            signal = Signal::PAUSE;
            break;
        case SID("F1") :
            signal = Signal::F1;
            break;
        case SID("F2") :
            signal = Signal::F2;
            break;
        case SID("F3") :
            signal = Signal::F3;
            break;
        case SID("F4") :
            signal = Signal::F4;
            break;
        case SID("F5") :
            signal = Signal::F5;
            break;
        case SID("F6") :
            signal = Signal::F6;
            break;
        case SID("F7") :
            signal = Signal::F7;
            break;
        case SID("F8") :
            signal = Signal::F8;
            break;
        case SID("F9") :
            signal = Signal::F9;
            break;
        case SID("F10") :
            signal = Signal::F10;
            break;
        case SID("F11") :
            signal = Signal::F11;
            break;
        case SID("F12") :
            signal = Signal::F12;
            break;
        case SID("F13") :
            signal = Signal::F13;
            break;
        case SID("F14") :
            signal = Signal::F14;
            break;
        case SID("F15") :
            signal = Signal::F15;
            break;
        case SID("F16") :
            signal = Signal::F16;
            break;
        case SID("F17") :
            signal = Signal::F17;
            break;
        case SID("F18") :
            signal = Signal::F18;
            break;
        case SID("F19") :
            signal = Signal::F19;
            break;
        case SID("F20") :
            signal = Signal::F20;
            break;
        case SID("F21") :
            signal = Signal::F21;
            break;
        case SID("F22") :
            signal = Signal::F22;
            break;
        case SID("F23") :
            signal = Signal::F23;
            break;
        case SID("F24") :
            signal = Signal::F24;
            break;
        case SID("F25") :
            signal = Signal::F25;
            break;
        case SID("KP0") :
            signal = Signal::KP0;
            break;
        case SID("KP1") :
            signal = Signal::KP1;
            break;
        case SID("KP2") :
            signal = Signal::KP2;
            break;
        case SID("KP3") :
            signal = Signal::KP3;
            break;
        case SID("KP4") :
            signal = Signal::KP4;
            break;
        case SID("KP5") :
            signal = Signal::KP5;
            break;
        case SID("KP6") :
            signal = Signal::KP6;
            break;
        case SID("KP7") :
            signal = Signal::KP7;
            break;
        case SID("KP8") :
            signal = Signal::KP8;
            break;
        case SID("KP9") :
            signal = Signal::KP9;
            break;
        case SID("KP_DECIMAL") :
            signal = Signal::KP_DECIMAL;
            break;
        case SID("KP_DIVIDE") :
            signal = Signal::KP_DIVIDE;
            break;
        case SID("KP_MULTIPLY") :
            signal = Signal::KP_MULTIPLY;
            break;
        case SID("KP_SUBTRACT") :
            signal = Signal::KP_SUBTRACT;
            break;
        case SID("KP_ADD") :
            signal = Signal::KP_ADD;
            break;
        case SID("KP_ENTER") :
            signal = Signal::KP_ENTER;
            break;
        case SID("KP_EQUAL") :
            signal = Signal::KP_EQUAL;
            break;
        case SID("LEFT_SHIFT") :
            signal = Signal::LEFT_SHIFT;
            break;
        case SID("LEFT_CTRL") :
            signal = Signal::LEFT_CTRL;
            break;
        case SID("LEFT_ALT") :
            signal = Signal::LEFT_ALT;
            break;
        case SID("LEFT_SUPER") :
            signal = Signal::LEFT_SUPER;
            break;
        case SID("RIGHT_SHIFT") :
            signal = Signal::RIGHT_SHIFT;
            break;
        case SID("RIGHT_CTRL") :
            signal = Signal::RIGHT_CTRL;
            break;
        case SID("RIGHT_ALT") :
            signal = Signal::RIGHT_ALT;
            break;
        case SID("RIGHT_SUPER") :
            signal = Signal::RIGHT_SUPER;
            break;
        case SID("MENU") :
            signal = Signal::MENU;
            break;
        case SID("MOUSE_1") :
        case SID("MOUSE_LEFT") :
            signal = Signal::MOUSE_1;
            break;
        case SID("MOUSE_2") :
        case SID("MOUSE_RIGHT") :
            signal = Signal::MOUSE_2;
            break;
        case SID("MOUSE_3") :
        case SID("MOUSE_MIDDLE") :
            signal = Signal::MOUSE_MIDDLE;
            break;
        case SID("MOUSE_4") :
            signal = Signal::MOUSE_4;
            break;
        case SID("MOUSE_5") :
            signal = Signal::MOUSE_5;
            break;
        case SID("MOUSE_6") :
            signal = Signal::MOUSE_6;
            break;
        case SID("MOUSE_7") :
            signal = Signal::MOUSE_7;
            break;
        case SID("MOUSE_8") :
            signal = Signal::MOUSE_8;
            break;
        case SID("MOUSE_MOVE_X") :
            signal = Signal::MOUSE_MOVE_X;
            break;
        case SID("MOUSE_MOVE_Y") :
            signal = Signal::MOUSE_MOVE_Y;
            break;
        case SID("MOUSE_SCROLL_UP") :
            signal = Signal::MOUSE_SCROLL_UP;
            break;
        case SID("MOUSE_SCROLL_DOWN") :
            signal = Signal::MOUSE_SCROLL_DOWN;
            break;
        case SID("MOUSE_SCROLL_RIGHT") :
            signal = Signal::MOUSE_SCROLL_RIGHT;
            break;
        case SID("MOUSE_SCROLL_LEFT") :
            signal = Signal::MOUSE_SCROLL_LEFT;
            break;
        case SID("GAMEPAD_A") :
        case SID("GAMEPAD_CROSS") :
            signal = Signal::GAMEPAD_A;
            break;
        case SID("GAMEPAD_B") :
        case SID("GAMEPAD_CIRCLE") :
            signal = Signal::GAMEPAD_B;
            break;
        case SID("GAMEPAD_X") :
        case SID("GAMEPAD_SQUARE") :
            signal = Signal::GAMEPAD_X;
            break;
        case SID("GAMEPAD_Y") :
        case SID("GAMEPAD_TRIANGLE") :
            signal = Signal::GAMEPAD_Y;
            break;
        case SID("GAMEPAD_LEFT_BUMPER") :
            signal = Signal::GAMEPAD_LEFT_BUMPER;
            break;
        case SID("GAMEPAD_RIGHT_BUMPER") :
            signal = Signal::GAMEPAD_RIGHT_BUMPER;
            break;
        case SID("GAMEPAD_BACK") :
            signal = Signal::GAMEPAD_BACK;
            break;
        case SID("GAMEPAD_START") :
            signal = Signal::GAMEPAD_START;
            break;
        case SID("GAMEPAD_GUIDE") :
            signal = Signal::GAMEPAD_GUIDE;
            break;
        case SID("GAMEPAD_LEFT_THUMB") :
            signal = Signal::GAMEPAD_LEFT_THUMB;
            break;
        case SID("GAMEPAD_RIGHT_THUMB") :
            signal = Signal::GAMEPAD_RIGHT_THUMB;
            break;
        case SID("GAMEPAD_DPAD_UP") :
            signal = Signal::GAMEPAD_DPAD_UP;
            break;
        case SID("GAMEPAD_DPAD_RIGHT") :
            signal = Signal::GAMEPAD_DPAD_RIGHT;
            break;
        case SID("GAMEPAD_DPAD_DOWN") :
            signal = Signal::GAMEPAD_DPAD_DOWN;
            break;
        case SID("GAMEPAD_DPAD_LEFT") :
            signal = Signal::GAMEPAD_DPAD_LEFT;
            break;
        case SID("GAMEPAD_AXIS_LEFT_X") :
            signal = Signal::GAMEPAD_AXIS_LEFT_X;
            break;
        case SID("GAMEPAD_AXIS_LEFT_Y") :
            signal = Signal::GAMEPAD_AXIS_LEFT_Y;
            break;
        case SID("GAMEPAD_AXIS_RIGHT_X") :
            signal = Signal::GAMEPAD_AXIS_RIGHT_X;
            break;
        case SID("GAMEPAD_AXIS_RIGHT_Y") :
            signal = Signal::GAMEPAD_AXIS_RIGHT_Y;
            break;
        case SID("GAMEPAD_AXIS_LEFT_TRIGGER") :
            signal = Signal::GAMEPAD_AXIS_LEFT_TRIGGER;
            break;
        case SID("GAMEPAD_AXIS_RIGHT_TRIGGER") :
            signal = Signal::GAMEPAD_AXIS_RIGHT_TRIGGER;
            break;
        
        default:
            signal = Signal::UNKNOWN;
            break;
    }

    return signal;
}

    
} // Atuin
