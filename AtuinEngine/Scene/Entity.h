
#pragma once


#include "Component.h"
#include "Core/Util/Types.h"
#include "Core/Util/StringFormat.h"
#include "Core/DataStructures/Array.h"
#include "Core/DataStructures/Map.h"

#include <bitset>
#include <string>


namespace Atuin {


class Transform;


class Entity {

    friend class Scene;


    enum class EntityState : U8{
        ACTIVE, 
        DIRTY,
        DESTROY,
        NUM_STATES
    };


public:

    // TODO move into Scene(Manager) ???
    static Entity* Instantiate(std::string_view name, Entity* parent = nullptr);
    static void    Destroy( Entity* entity);


    ~Entity();

    std::string Name() { return mName; }
    void SetName( std::string_view newName) { mName = newName; }

    
    // template<class T>
    void AddComponent( Component *newComponent);
    template <class T>
    T* GetComponent();
    template <class T>
    Array<T*>& GetAllComponents();


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


    Transform* transform;

private:

    Entity( std::string_view name);


    std::string mName;
    Map<U64, Array<Component*>> mComponents;
    // Array<U64> mComponentTypes;
    // Array<Component*> mComponents;

    std::bitset<(Size)EntityState::NUM_STATES> mState;

};

    
template <class T>
T* Entity::GetComponent() {

    if constexpr ( !std::is_base_of<Component, T>::value) 
    {
        throw std::logic_error( FormatStr("Entity::GetComponent() => Class %s is not derived from class Component.",  typeid(T).name()) );
    } 

    U64 typeId = SID( T::Type().c_str);
    if ( mComponents.Find( typeId) == mComponents.End())
    {
        throw std::runtime_error( FormatStr("Entity::GetComponent() => Entity %s has no component of type %s.", mName, T::Type()) );
    }

    return mComponents[ typeId].Back();

    // U64 typeID = SID( T::Type());
    // Size numComponents = mComponents.GetSize();
    // for ( Size i = 0; i < numComponents; i++)
    // {
    //     if ( mComponentTypes[i] == typeID )
    //     {
    //         return static_cast<T*>( mComponents[i]);
    //     }
    // }

    // return nullptr;
}

    
template <class T>
Array<T*>& Entity::GetAllComponents() {

    if constexpr ( !std::is_base_of<Component, T>::value) 
    {
        throw std::logic_error( FormatStr("Entity::GetAllComponents() => Class %s is not derived from class Component.",  typeid(T).name()) );
    } 

    U64 typeId = SID( T::Type().c_str);
    if ( mComponents.Find( typeId) == mComponents.End())
    {
        throw std::runtime_error( FormatStr("Entity::GetAllComponents() => Entity %s has no component of type %s.", mName, T::Type()) );
    }

    return mComponents[ typeId];

    // U64 typeID = SID( T::Type());
    // Array<T*> res;
    // Size numComponents = mComponents.GetSize();
    // for ( Size i = 0; i < numComponents; i++)
    // {
    //     if ( mComponentTypes[i] == typeID )
    //     {
    //         return res.Push_Back( static_cast<T*>( mComponents[i]));
    //     }
    // }

    // return res;
}

    
} // Atuin
