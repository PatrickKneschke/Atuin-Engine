
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

    GLFWwindow* Window()       const { return pWindow; }
    std::string WindowTitle()  const { return mTitle; }
    I32         WindowWidth()  const { return mWidth; }
    I32         WindowHeight() const { return mHeight; }
    bool        InFullscreen() const { return mFullscreen; }

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