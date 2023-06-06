
#include "KeyBinding.h"

#include "GLFW/glfw3.h"


namespace Atuin {


void KeyBinding::RegisterKeyCallback(int key, int mods, int action, std::function<void()> callback) {

    switch (action)
    {
        case GLFW_PRESS:
            mKeyPress[std::make_pair(key, mods)].push_back(callback);
            break;
        case GLFW_RELEASE:
            mKeyRelease[std::make_pair(key, mods)].push_back(callback);
            break;        
        default:
            break;
    }
}


void KeyBinding::RegisterMouseCallback(int button, int mods, int action, std::function<void()> callback) {

    switch (action)
    {
        case GLFW_PRESS:
            mKeyPress[std::make_pair(button, mods)].push_back(callback);
            break;
        case GLFW_RELEASE:
            mKeyRelease[std::make_pair(button, mods)].push_back(callback);
            break;        
        default:
            break;
    }
}


void KeyBinding::InvokeKeyPress(int key, int mods) {

    auto &callbacks = mKeyPress[{key, mods}];
    for (auto cb : callbacks)
    {
        cb();
    }
}


void KeyBinding::InvokeKeyRelease(int key, int mods) {

    auto &callbacks = mKeyRelease[{key, mods}];
    for (auto cb : callbacks)
    {
        cb();
    }
}


void KeyBinding::InvokeMousePress(int button, int mods) {

    auto &callbacks = mMousePress[{button, mods}];
    for (auto cb : callbacks)
    {
        cb();
    }
}


void KeyBinding::InvokeMouseRelease(int button, int mods) {

    auto &callbacks = mMouseRelease[{button, mods}];
    for (auto cb : callbacks)
    {
        cb();
    }
}


} // Atuin
