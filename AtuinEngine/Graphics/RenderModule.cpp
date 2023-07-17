
#include "RenderModule.h"
#include "Core/Util/Types.h"

#include "GLFW/glfw3.h"

#include <vector>


namespace Atuin {
    

RenderModule::RenderModule() : pWindow {nullptr}, mRenderer() {


}


RenderModule::~RenderModule() {


}


void RenderModule::StartUp(GLFWwindow *window) {

    pWindow = window;
    mRenderer.StartUp(pWindow);
}


void RenderModule::ShutDown() {

    mRenderer.ShutDown();
}


void RenderModule::Update() {

    mRenderer.Update();
}


} // Atuin
