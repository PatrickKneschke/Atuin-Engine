
#pragma once


#include <functional>

#include <stack>
#include <list>


class GLFWwindow;

namespace Atuin {


class EngineLoop;
class KeyBinding;

class InputModule {

public:

    InputModule(EngineLoop *engine);
    ~InputModule();

    void StartUp(GLFWwindow *window);
    void ShutDown();
    void Update();

    void AddKeyBinding(KeyBinding *keyBinding);
    void RemoveKeyBinding();

    bool IsKeyPressed(int key) const;
    bool IsMouseButtonPressed(int button) const;
    std::pair<double, double> GetCursorPos() const;


private:

    static void KeyListener(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void MouseListener(GLFWwindow *window, int button, int action, int mods);
    static void CursorPosListener(GLFWwindow *window, double xpos, double ypos);

    static void WindowCloseListener(GLFWwindow *window);
    static void WindowSizeListener(GLFWwindow *window, int width, int height);

    static GLFWwindow* sWindow;
    static bool sDisableInput;
    static std::stack<KeyBinding*> sKeyBindings; // TODO replace with custom stack

    static std::list<std::function<void(GLFWwindow*, double, double)>>  sCursorPosCallbacks;
    static std::list<std::function<void(GLFWwindow*)>>                  sWindowCloseCallbacks;
    static std::list<std::function<void(GLFWwindow*, int, int)>>        sWindowSizeCallbacks;

    EngineLoop* pEngine;
};

    
} // Atuin
