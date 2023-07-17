
#include "Renderer.h"
#include "RendererCore.h"
#include "Core/Util/Types.h"

#include "GLFW/glfw3.h"


namespace Atuin {


Renderer::Renderer() : 
	mLog(), 
	mMemory(), 
	mJobs(), 
	pWindow {nullptr}, 
	pCore {nullptr} 
{


}


Renderer::~Renderer() {


}


void Renderer::StartUp(GLFWwindow *window) {

	pWindow = window;
	pCore = mMemory.New<RendererCore>(pWindow);
}


void Renderer::ShutDown() {

	mMemory.Delete(pCore);
}
    

void Renderer::Update() {


}


    
} // Atuin
