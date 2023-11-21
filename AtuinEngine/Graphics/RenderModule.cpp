
#include "RenderModule.h"
#include "Core/Util/Types.h"

#include "GLFW/glfw3.h"


namespace Atuin {
    

RenderModule::RenderModule() : pWindow {nullptr}, mRenderer() {


}


RenderModule::~RenderModule() {


}


void RenderModule::StartUp(GLFWwindow *window) {

    pWindow = window;
    mRenderer.StartUp(pWindow);


	CreateScene();
}


void RenderModule::ShutDown() {

    mRenderer.ShutDown();
}


void RenderModule::Update() {

    // UpdateObjects();

    mRenderer.Update();

    // late update
    // UpdateScene();
}


} // Atuin
