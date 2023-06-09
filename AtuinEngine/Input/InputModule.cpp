
#include "InputModule.h"
#include "KeyBinding.h"

#include "GLFW/glfw3.h"

#include <iostream>


namespace Atuin {


GLFWwindow* InputModule::sWindow = nullptr;
bool InputModule::sDisableInput = false;
std::stack<KeyBinding*> InputModule::sKeyBindings;
std::list<std::function<void(GLFWwindow*, double, double)>> InputModule::sCursorPosCallbacks;
std::list<std::function<void(GLFWwindow*)>>                 InputModule::sWindowCloseCallbacks;
std::list<std::function<void(GLFWwindow*, int, int)>>       InputModule::sWindowSizeCallbacks;


InputModule::InputModule(EngineLoop *engine) : pEngine {engine} {

}


InputModule::~InputModule() {

}


void InputModule::StartUp(GLFWwindow *window) {

    sWindow = window;
    
    glfwSetInputMode(sWindow, GLFW_STICKY_KEYS, 1);
    glfwSetInputMode(sWindow, GLFW_STICKY_MOUSE_BUTTONS, 1);

    glfwSetKeyCallback(sWindow, KeyListener);
    glfwSetMouseButtonCallback(sWindow, MouseListener);

    glfwSetCursorPosCallback(sWindow, CursorPosListener);
    glfwSetWindowSizeCallback(sWindow, WindowSizeListener);
    glfwSetWindowCloseCallback(sWindow, WindowCloseListener);
}


void InputModule::ShutDown() {
}


void InputModule::Update() {

    glfwPollEvents();
}


void InputModule::AddKeyBinding(KeyBinding *keyBinding) {

    sKeyBindings.push(keyBinding);
}


void InputModule::RemoveKeyBinding() {

    sKeyBindings.pop();
}


bool InputModule::IsKeyPressed(int key) const {

    return glfwGetKey(sWindow, key) == GLFW_PRESS;
}


bool InputModule::IsMouseButtonPressed(int button) const {

    return glfwGetMouseButton(sWindow, button) == GLFW_PRESS;
}


std::pair<double, double> InputModule::GetCursorPos() const {

    double x, y;
    glfwGetCursorPos(sWindow, &x, &y);

    return std::make_pair(x, y);
}


void InputModule::KeyListener(GLFWwindow*, int key, int, int action, int mods) {

    if (sKeyBindings.empty())
    {
        return;
    }

    auto currBinding = sKeyBindings.top();
    switch (action)
    {
        case GLFW_PRESS:
            currBinding->InvokeKeyPress(key, mods);
            break;        
        case GLFW_RELEASE:
            currBinding->InvokeKeyRelease(key, mods);
            break;

        default:
            break;
    }
}


void InputModule::MouseListener(GLFWwindow* , int button, int action, int mods) {

    if (sKeyBindings.empty())
    {
        return;
    }

    auto currBinding = sKeyBindings.top();
    switch (action)
    {
        case GLFW_PRESS:
            currBinding->InvokeKeyPress(button, mods);
            break;        
        case GLFW_RELEASE:
            currBinding->InvokeKeyRelease(button, mods);
            break;

        default:
            break;
    }
}


void InputModule::CursorPosListener(GLFWwindow *window, double xpos, double ypos) {

    for (auto &cb : sCursorPosCallbacks)
    {
        cb(window, xpos, ypos);
    }
}


void InputModule::WindowCloseListener(GLFWwindow *window) {

    for (auto &cb : sWindowCloseCallbacks)
    {
        cb(window);
    }
}


void InputModule::WindowSizeListener(GLFWwindow *window, int width, int height) {

    for (auto &cb : sWindowSizeCallbacks)
    {
        cb(window, width, height);
    }
}

    
} // Atuin
