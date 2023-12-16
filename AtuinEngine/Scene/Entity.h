
#pragma once


#include "Core/Util/Types.h"
#include "Core/Util/StringFormat.h"
#include "Core/DataStructures/Array.h"
#include "Core/DataStructures/Map.h"

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
    static void    Destroy( Entity* entity);


    ~Entity();

    std::string Name() { return mName; }
    void SetName( std::string_view newName) { mName = newName; }

    bool GetState( EntityState state) const;
    void SetState( EntityState state, bool value);

    bool IsActive() const;
    void SetActive( bool value);

    // life cycle 
    void OnEnable();
    void OnDisable();
    void OnDestroy();

    void Update();
    void FixedUpdate();
    void LateUpdate();

    
    template<class T>
    void AddComponent( T *newComponent);
    template <class T>
    T* GetComponent();


    Transform* transform;

private:

    Entity( std::string_view name);


    std::string mName;
    Map<U64, Array<Component*>> mComponents;

    std::bitset<(Size)EntityState::NUM_STATES> mState;

};


template <class T>
void Entity::AddComponent( T *newComponent) {

    if constexpr ( !std::is_base_of<Component, T>::value) 
    {
        throw std::logic_error( FormatStr("Entity::AddComponent => Class %s is not derived from class Component.",  typeid(T).name()) );
    } 

    U64 typeIdx = SID( T::Type());
    if ( T::IsUnique( T::Type())  &&  mComponents.Find( typeIdx) != mComponents.End() )
    {
        throw std::runtime_error( FormatStr("Entity::AddComponent => Entity %s already has component of unique type %s.", mName, T::Type()) );
    }

    mComponents[ typeIdx].PushBack( newComponent);
    (Entity *&)(newComponent->entity) = this;
    newComponent->SetActive( IsActive());
}

    
template <class T>
T* Entity::GetComponent() {

    if constexpr ( !std::is_base_of<Component, T>::value) 
    {
        throw std::logic_error( FormatStr("Entity::GetComponent => Class %s is not derived from class Component.",  typeid(T).name()) );
    } 

    U64 typeIdx = SID( T::Name().c_str());
    if ( mComponents.Find( typeIdx) == mComponents.End())
    {
        throw std::runtime_error( FormatStr("Entity::GetComponent => Entity %s has no component of type %s.", mName, T::Type()) );
    }

    return mComponents[ typeIdx].Back();
}
    
} // Atuin
