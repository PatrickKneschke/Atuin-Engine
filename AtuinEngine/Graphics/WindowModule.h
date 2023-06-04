
#pragma once


#include "Core/Config/CVar.h"

#include "GLFW/glfw3.h"


namespace Atuin {


class EngineLoop;

class WindowModule {

public:

    WindowModule(EngineLoop *engine);
    ~WindowModule();

    void StartUp();
    void ShutDown();
    void Update();


private:

    static CVar<std::string>*   pWindowTitle;
    static CVar<I32>*           pWindowWidth;
    static CVar<I32>*           pWindowHeight;
    static CVar<bool>*          pSetFullscreen; 
    

    GLFWwindow* pWindow;
    std::string mTitle;
    I32         mWidth;
    I32         mHeight;
    bool        mFullscreen;

    EngineLoop* pEngine;
};


} // Atuin