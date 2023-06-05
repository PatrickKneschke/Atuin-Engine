
#pragma once


#include "Core/Util/Types.h"

#include <functional>

#include <list>
#include <unordered_map>


namespace Atuin {


class KeyBinding {

    using InputMap = std::unordered_map<std::pair<int, int>, std::list<std::function<void()>>>; // (key, mod) -> callbacks list

public:
    
    KeyBinding() = default;
    ~KeyBinding() = default;

    void RegisterKeyCallback(int key, int mod, int action);
    void RegisterMouseCallback(int button, int mod, int action);

    InputMap mKeyPress;
    InputMap mKeyRepeat;
    InputMap mKeyRelease;
    InputMap mMousePress;
    InputMap mMouseRepeat;
    InputMap mMouseRelease;
    // InputMap mMouseMove;
    // InputMap mMouseScroll;

};

    
} // Atuin
