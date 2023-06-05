
#pragma once


#include <stack>


namespace Atuin {


class EngineLoop;
class GLFWwindow;
class KeyBinding;

class InputModule {

public:

    InputModule(EngineLoop *engine);
    ~InputModule();

    void StartUp(GLFWwindow *window);
    void ShutDown();
    void Update();


private:

    static void KeyListener(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void MouseListener(GLFWwindow *window, int button, int action, int mods);
    static void CursorPosListener(GLFWwindow *window, double xpos, double ypos);
    static void WindowCloseListener(GLFWwindow *window);
    static void WindowSizeListener(GLFWwindow *window, int width, int height);


    std::stack<KeyBinding*> mKeyBindings; // TODO replace with custom stack

    GLFWwindow* pWindow;
    EngineLoop* pEngine;
};

    
} // Atuin
