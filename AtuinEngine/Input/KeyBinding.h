
#pragma once


#include "Core/Util/Types.h"

#include <functional>

#include <list>
#include <unordered_map>


namespace Atuin {


class KeyBinding {

    using InputMap = std::unordered_map<std::pair<int, int>, std::list<std::function<void()>>, PairHash>; // (key, mods) -> callbacks list

public:
    
    KeyBinding() = default;
    ~KeyBinding() = default;

    void RegisterKeyCallback(int key, int mods, int action, std::function<void()> callback);
    void RegisterMouseCallback(int button, int mods, int action, std::function<void()> callback);

    void InvokeKeyPress(int key, int mods);
    void InvokeKeyRelease(int key, int mods);
    void InvokeMousePress(int button, int mods);
    void InvokeMouseRelease(int button, int mods);

    InputMap mKeyPress;
    InputMap mKeyRelease;
    InputMap mMousePress;
    InputMap mMouseRelease;
    // InputMap mMouseMove;
    // InputMap mMouseScroll;

};

    
} // Atuin
