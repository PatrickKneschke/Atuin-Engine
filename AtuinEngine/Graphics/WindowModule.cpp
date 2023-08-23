
#include "WindowModule.h"
#include "App.h"
#include "Core/Config/ConfigManager.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"


namespace Atuin {


CVar<std::string>* WindowModule::pWindowTitle   = ConfigManager::RegisterCVar("Window", "WINDOW_TITLE", std::string("MainWindow"));
CVar<I32>* WindowModule::pWindowWidth           = ConfigManager::RegisterCVar("Window", "WINDOW_WIDTH", 1920);
CVar<I32>* WindowModule::pWindowHeight          = ConfigManager::RegisterCVar("Window", "WINDOW_HEIGHT", 1080);
CVar<bool>* WindowModule::pSetFullscreen        = ConfigManager::RegisterCVar("Window", "SET_FULLSCREEN", false);

    

WindowModule::WindowModule() : 
    mTitle {pWindowTitle->Get()}, 
    mWidth {pWindowWidth->Get()}, 
    mHeight {pWindowHeight->Get()}, 
    mLog()
{

}
    

WindowModule::~WindowModule() {

}


void WindowModule::StartUp() {

    // init glfw
    if (!glfwInit())
    {
        mLog.Error(LogChannel::GRAPHICS, "Failed to init glfw.");
    }

    // configure GLFW window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // for vulkan
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    // create window
    pWindow = glfwCreateWindow(mWidth, mHeight, mTitle.c_str(), nullptr, nullptr);
    if (pWindow == nullptr)
    {
        mLog.Error(LogChannel::GRAPHICS, "Failed to create glfw window.");
    }    

    // glfwSetWindowUserPointer(pWindow, this);
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

    if (glfwWindowShouldClose(pWindow)) 
    {
        App::Quit();
    }
    glfwSwapBuffers(pWindow);
}


} // Atuin