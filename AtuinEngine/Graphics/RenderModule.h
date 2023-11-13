
#pragma once


#include "Renderer/Renderer.h"


#include <unordered_set>
#include <glm/gtx/string_cast.hpp>


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
    Map<MeshObject*, int> modifiedObjects;
    std::unordered_set<U32> mReuseObjectIndices;
    std::string materials[5] = {
        // "Materials//Rubber_Hose/rubber_hose.material.json"
        "Materials//Rusted_Iron/rusted_iron.material.json", 
        "Materials//Rusted_Iron/rusted_iron.material.json", 
        "Materials//Rusted_Iron/rusted_iron.material.json", 
        "Materials//Rusted_Iron/rusted_iron.material.json"
    };
    std::string models[3] = {
        "Meshes/Default/cube.obj", 
        "Meshes/Default/sphere.obj", 
        "Meshes/Default/torus.obj"
    };


    void CreateScene() {

        int N = 1;
        float unit = 3.0f;
        mTestObjects.Reserve( N*N*N + 1);
        for ( int i = 0; i < N; i++)
        {
            for ( int j = 0; j < N; j++)
            {
                for ( int k = 0; k < N; k++)
                {
                    int materialIdx = std::rand() % 5;
                    int modelIdx = 1;//std::rand() % 3;
                    glm::vec3 position = glm::vec3( (float)i * unit,  (float)j * unit, (float)k * unit);

                    MeshObject obj;
                    obj.materialName = materials[ materialIdx];
                    obj.meshName     = models[ modelIdx];
                    obj.transform    = glm::translate( glm::mat4(1.f), position);
                    obj.sphereBounds = glm::vec4( position, modelIdx == 0 ? 1.7321f : 1.f);
                    mTestObjects.PushBack( obj);          
                    
                    mRenderer.RegisterMeshObject( &mTestObjects.Back());
                    ++modifiedObjects[ &mTestObjects.Back()];
                }                
            }
        }

        MeshObject obj;
        glm::vec3 position = glm::vec3( 0.f,  -102.f, 0.f);
        glm::vec3 scale = glm::vec3( 100.f,  100.f, 100.f);
        obj.materialName = "Materials//Rusted_Iron/rusted_iron.material.json";
        obj.meshName     = "Meshes/Default/cube.obj";
        obj.transform    = glm::scale(glm::translate( glm::mat4(1.f), position), scale);
        obj.sphereBounds = glm::vec4( position, 100 * 1.7321f);
        mTestObjects.PushBack( obj);          
                    
        mRenderer.RegisterMeshObject( &mTestObjects.Back());


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

    void UpdateScene() {

        modifiedObjects.Clear();

        if ( mRenderer.FrameCount() % 60 == 1 )
        {
            U32 turnOver = (U32)ceil( 0.1 * (U32)mTestObjects.GetSize());
            // add some objects
            for( U32 i=0; i<turnOver && !mReuseObjectIndices.empty(); i++)
            {
                glm::vec3 position = glm::vec3( 
                    8.f * (float)(std::rand() % 10),
                    8.f * (float)(std::rand() % 10),
                    8.f * (float)(std::rand() % 10)
                );

                int materialIdx = std::rand() % 5;

                MeshObject obj;
                obj.materialName = materials[ materialIdx];
                obj.meshName     = "Meshes/Default/torus.obj";
                obj.transform    = glm::translate( glm::mat4(1.f), position);
                obj.sphereBounds = glm::vec4( position, 1.0f);
                   

                U32 idx;
                if ( mReuseObjectIndices.empty())
                {
                    idx = (U32)mTestObjects.GetSize();
                    mTestObjects.PushBack( obj);
                }
                else
                {
                    idx = *mReuseObjectIndices.begin();
                    mReuseObjectIndices.erase( mReuseObjectIndices.begin());
                    mTestObjects[ idx] = obj;
                }

                mRenderer.RegisterMeshObject( &mTestObjects[ idx]);
                ++modifiedObjects[ &mTestObjects[ idx]];
            }

            // delete some objects
            U32 i = 0;
            while ( i<turnOver)
            {
                U32 idx = rand() % (U32)mTestObjects.GetSize();
                if ( mReuseObjectIndices.find( idx) == mReuseObjectIndices.end() && modifiedObjects[ &mTestObjects[ idx]] == 0 )
                {
                    mRenderer.DeleteMeshObject( &mTestObjects[ idx]);
                    mReuseObjectIndices.insert( idx);
                }
                ++i;
            }
        }
        
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
            
            mRenderer.UpdateMeshObject( &mTestObjects[ idx]);
        }
    }
};

    
} // Atuin
