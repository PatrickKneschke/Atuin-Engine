
#include "WindowModule.h"
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Debug/Logger.h"


namespace Atuin {


CVar<std::string>* WindowModule::pWindowTitle   = ConfigManager::RegisterCVar("Window", "WINDOW_TITLE", std::string("MainWindow"));
CVar<I32>* WindowModule::pWindowWidth           = ConfigManager::RegisterCVar("Window", "WINDOW_WIDTH", 1920);
CVar<I32>* WindowModule::pWindowHeight          = ConfigManager::RegisterCVar("Window", "WINDOW_HEIGHT", 1080);
CVar<bool>* WindowModule::pSetFullscreen        = ConfigManager::RegisterCVar("Window", "SET_FULLSCREEN", false);

    

WindowModule::WindowModule(EngineLoop *engine) : 
    mTitle {pWindowTitle->Get()}, 
    mWidth {pWindowWidth->Get()}, 
    mHeight {pWindowHeight->Get()}, 
    pEngine {engine} 
{

}
    

WindowModule::~WindowModule() {

}


void WindowModule::StartUp() {

    // init glfw
    if (!glfwInit())
    {
        pEngine->Log()->Error(LogChannel::GRAPHICS, "Failed to init glfw.");
    }

    // configure GLFW window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // for vulkan
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    // create window
    pWindow = glfwCreateWindow(1200, 800, "Window Title", nullptr, nullptr);
    if (pWindow == nullptr)
    {
        pEngine->Log()->Error(LogChannel::GRAPHICS, "Failed to create glfw window.");
    }    

    glfwSetWindowUserPointer(pWindow, this);
    glfwMakeContextCurrent(pWindow);
    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}


void WindowModule::ShutDown() {

    if (pWindow != nullptr)
    {
        glfwDestroyWindow(pWindow);
    }
    
    glfwTerminate();
}


void WindowModule::Update() {

    // TODO move to InputModule
    glfwPollEvents();

    if (glfwWindowShouldClose(pWindow)) 
    {
        pEngine->Quit();
    }
    glfwSwapBuffers(pWindow);
}


} // Atuin