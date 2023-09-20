
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


    mTestObject.materialName = "Materials//Rusted_Iron/rusted_iron.material.json";
    mTestObject.meshName = "Meshes/Default/torus.obj";
	mTestObject.transform = glm::mat4(1.f);
    mTestObject.objectIdx = mRenderer.RegisterMeshObject( mTestObject);
}


void RenderModule::ShutDown() {

    mRenderer.ShutDown();
}


void RenderModule::Update() {

    
    glm::mat4 rotation = glm::rotate(
		glm::mat4(1.f), 
		(float) mRenderer.FrameCount() / 60.f * glm::radians(30.f), 
		glm::vec3(0.5f, 1.f, 0.75f)
	);
	glm::mat4 translate = glm::translate(
		glm::mat4(1.f),
		glm::vec3(0.f, 0.f, 0.f)
	);
	glm::mat4 scale = glm::scale(
		glm::mat4(1.f),
		glm::vec3(1.f, 1.f, 1.f)
	);
	mTestObject.transform = translate * rotation * scale;
	mRenderer.UpdateMeshObject( mTestObject.objectIdx);


    mRenderer.Update();
}


} // Atuin
