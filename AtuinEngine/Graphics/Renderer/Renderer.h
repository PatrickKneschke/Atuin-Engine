
#pragma once


#include "Definitions.h"
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

	void CreateDepthResources();
    void CreateRenderPass();


    Log mLog;
    Memory mMemory;
    Jobs mJobs;

    GLFWwindow* pWindow;
    RendererCore* pCore;

    Swapchain mSwapchain;
    ImageResource mDepthImage;
    vk::RenderPass mRenderPass;
    
};


} // Atuin
