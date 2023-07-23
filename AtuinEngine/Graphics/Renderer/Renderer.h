
#pragma once


#include "Core/Debug/Log.h"
#include "Core/Memory/Memory.h"
#include "Core/Jobs/Jobs.h"


class GLFWwindow;

namespace Atuin {


    

class RendererCore;

class Renderer {

public:

    Renderer();
    ~Renderer();


    void StartUp(GLFWwindow* window);
    void ShutDown();
    void Update();


private:

    Log mLog;
    Memory mMemory;
    Jobs mJobs;

    GLFWwindow* pWindow;
    RendererCore* pCore;

};


} // Atuin