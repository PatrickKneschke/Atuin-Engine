
#pragma once


#include "Core/DataStructures/Json.h"


namespace Atuin {


class Entity;

class Scene {


public:

    Scene() : pSceneRoot {nullptr}, mIsLoaded {false} {}
    ~Scene() = default;

    void Load( Json sceneData);
    void Unload();

    bool IsLoaded() { return mIsLoaded; }

    void Update();
    void FixedUpdate();
    void LateUpdate();

private:

    // called from LateUpdate(), removes all entities destroyed this frame
    void CleanUp();

    Entity* pSceneRoot;
    bool mIsLoaded;

};


    
} // Atuin
