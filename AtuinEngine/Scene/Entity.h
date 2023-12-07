
#pragma once


#include "Core/DataStructures/Array.h"
#include "Core/DataStructures/Map.h"

#include <string>


namespace Atuin {


class Transform;
class Component;


class Entity {


    enum class EntityAttributes {
        ACTIVE, 
        DIRTY,
        DESTROY
    };


public:

    static Entity* Instantiate(std::string_view name, Entity* parent = nullptr);
    
    ~Entity();

    std::string Name() { return mName; }
    void SetName( std::string_view newName) { mName = newName; }

    // life cycle 
    void OnEnable();
    void OnDisable();
    void Destroy();

    void Update();
    void FixedUpdate();
    void LateUpdate();


    Transform* transform;

private:

    Entity( std::string_view name);


    std::string mName;
    Map<U64, Component*> pComponents;
    // Array<Component*> pComponents;

};

    
} // Atuin
