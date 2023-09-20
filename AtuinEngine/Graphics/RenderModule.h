
#pragma once


#include "Renderer/Renderer.h"


class GLFWwindow;


namespace Atuin {


class RenderModule {

public:

    RenderModule();
    ~RenderModule();

    void StartUp(GLFWwindow *window);
    void ShutDown();
    void Update();


private:

    GLFWwindow* pWindow;
    Renderer mRenderer;


    MeshObject mTestObject;
};

    
} // Atuin
