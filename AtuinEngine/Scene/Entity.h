
#pragma once


#include "Core/Util/Types.h"
#include "Core/DataStructures/Array.h"

#include <bitset>
#include <string>


namespace Atuin {


class Transform;
class Component;


class Entity {


    enum class EntityState : U8{
        ACTIVE, 
        DIRTY,
        DESTROY,
        NUM_STATES
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
    Array<Component*> pComponents;

    std::bitset<(Size)EntityState::NUM_STATES> mState;

};

    
} // Atuin
