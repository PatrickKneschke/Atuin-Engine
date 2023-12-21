
#pragma once


#include "Core/DataStructures/Json.h"
#include "Core/DataStructures/Map.h"

#include <string>


namespace Atuin {


class Entity;

class Scene {


public:

    Scene() : pSceneRoot {nullptr}, mIsLoaded {false} {}
    ~Scene() = default;

    void Load( const Json &sceneData);
    void Unload();

    bool IsLoaded() { return mIsLoaded; }

    void Update();
    void FixedUpdate();
    void LateUpdate();

private:

    void LoadEntity( std::string_view entityName, const Json &entityData);

    // called from LateUpdate(), removes all entities destroyed this frame
    void CleanUp();

    Entity* pSceneRoot;
    bool mIsLoaded;
    Map<U64, Entity*> mEntities;

};

    
} // Atuin
