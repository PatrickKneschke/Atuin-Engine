
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


   // just for testing

   Array<MeshObject> mTestObjects;

    void CreateObjects() {

        std::string models[3] = {
            "Meshes/Default/cube.obj", "Meshes/Default/sphere.obj", "Meshes/Default/torus.obj"
        };

        int N = 20;
        mTestObjects.Reserve( N*N*N);
        for ( int i = 0; i < N; i++)
        {
            for ( int j = 0; j < N; j++)
            {
                for ( int k = 0; k < N; k++)
                {
                    int modelIdx = std::rand() % 3;
                    glm::vec3 position = glm::vec3( i * 4.f, j * 4.f, k * 4.f);

                    MeshObject obj;
                    obj.materialName = "Materials//Rusted_Iron/rusted_iron.material.json";
                    obj.meshName     = models[ modelIdx];
                    obj.transform    = glm::translate( glm::mat4(1.f), position);
                    obj.sphereBounds = glm::vec4( position, 1.f);
                    mTestObjects.PushBack( obj);          
                    
                    mRenderer.RegisterMeshObject( mTestObjects.Back());
                }                
            }
        }
    }


    void UpdateObjects() {

        int numObjects = mTestObjects.GetSize();
        int numUpdates = ceil( numObjects * 0.2f);
        for (int i = 0; i < numUpdates; i++)
        {
            int idx = rand() % numObjects;

            mTestObjects[ idx].transform = glm::rotate(
                mTestObjects[ idx].transform, 
                1.f / 30.f * glm::radians(30.f), 
                glm::vec3(0.5f, 1.f, 0.75f)
            );
            
            mRenderer.UpdateMeshObject( mTestObjects[ idx].objectIdx);
        }
    }
};

    
} // Atuin
