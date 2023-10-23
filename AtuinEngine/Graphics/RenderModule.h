
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
        float unit = 8.0f;
        mTestObjects.Reserve( N*N*N);
        for ( int i = 0; i < N; i++)
        {
            for ( int j = 0; j < N; j++)
            {
                for ( int k = 0; k < N; k++)
                {
                    int modelIdx = std::rand() % 3;
                    glm::vec3 position = glm::vec3( (float)i * unit,  (float)j * unit, (float)k * unit);

                    MeshObject obj;
                    obj.materialName = "Materials//Rusted_Iron/rusted_iron.material.json";
                    obj.meshName     = models[ modelIdx];
                    obj.transform    = glm::translate( glm::mat4(1.f), position);
                    obj.sphereBounds = glm::vec4( position, modelIdx == 0 ? 1.7321f : 1.f);
                    mTestObjects.PushBack( obj);          
                    
                    mRenderer.RegisterMeshObject( mTestObjects.Back());
                }                
            }
        }

        // int modelIdx = 1;
        // glm::vec3 position = glm::vec3( 400.f, 400.f, 400.f);

        // MeshObject obj;
        // obj.materialName = "Materials//Rusted_Iron/rusted_iron.material.json";
        // obj.meshName     = models[ modelIdx];
        // obj.transform    = glm::translate( glm::mat4(1.f), position);
        // obj.sphereBounds = glm::vec4( position, 1.f);
        // mTestObjects.PushBack( obj);          
                   
        // mRenderer.RegisterMeshObject( mTestObjects.Back());
    }


    void UpdateObjects() {

        U32 numObjects = (U32)mTestObjects.GetSize();
        U32 numUpdates = (U32)ceil( 0.2 * numObjects);
        for (U32 i = 0; i < numUpdates; i++)
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
