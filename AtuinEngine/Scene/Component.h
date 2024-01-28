
#pragma once


#include "Core/Util/Types.h"
#include "Core/Util/StringID.h"
#include "Core/Util/StringFormat.h"
#include "Core/DataStructures/Map.h"
#include "Core/DataStructures/Json.h"
#include "Core/Memory/MemoryManager.h"

#include <bitset>
#include <string>


#define DEFINE_COMPONENT(NAME, BASE, UNIQUE)                                    \
    template<bool Unique>                                                       \
    class ComponentRegistry<class NAME, BASE, Unique> {                         \
        protected:                                                              \
            static bool NAME##Registered;                                       \
    };                                                                          \
    class NAME : public BASE, public ComponentRegistry<NAME, BASE, UNIQUE> {    \
        public:                                                                 \
            static Component* Instantiate();                                    \
            static std::string Type() { return #NAME; }                         \
            U64 TypeID() const override {                                       \
                static U64 id = SID( #NAME);                                    \
                return id;                                                      \
            }                                                                   \
            bool IsUnique() const override { return UNIQUE; }                   \
        protected:                                                              \
            static bool isRegistered() { return NAME##Registered; }             \
                                                                                \
        private:

#define DEFINE_COMPONENT_END(NAME, BASE, UNIQUE)                                \
    };                                                                          \
    template<>                                                                  \
    bool ComponentRegistry<NAME, BASE, UNIQUE>::NAME##Registered = Component::RegisterComponent<NAME>( #NAME, UNIQUE);



namespace Atuin {


template<class Derived, class Base, bool Unique> 
class ComponentRegistry{};


class Entity;


class Component {


    using PCreateFunc = Component* (*)();


    friend class Entity;
    friend class Scene;
    friend class MemoryManager;


protected:  

    enum class ComponentState : U8 {
        ACTIVE,
        AWAKE,
        DESTROY,
        UPDATE,
        FIXED_UPDATE,
        LATE_UPDATE,
        NUM_STATES
    };


public:

    template<class Derived>
    static bool RegisterComponent( std::string componentName, bool isUnique) {

        U64 componentTypeIdx = SID(componentName.c_str());
        // add component constructor
        sConstructors[ componentTypeIdx] = &CreateFunc<Derived>;

        // add to component type dict
        sComponentTypes[ Derived::Type()] = isUnique;

        return true;
    }

    static Map<std::string, bool>& GetComponentTypes() {

        return sComponentTypes;
    }

    virtual ~Component() = default;

    bool IsActive() const;
    void SetActive( bool value);

    bool GetState( ComponentState state) const;
    void SetState( ComponentState state, bool value);

    // automatically overridden upon derived class declaration using DEFINE_COMPONENT macro

    virtual U64 TypeID() const = 0;
    virtual bool IsUnique() const  = 0;

    // overridden manually

    virtual void Init( Json data) = 0;
    
    virtual void OnEnable() = 0;
    virtual void Awake() = 0;
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void FixedUpdate() = 0;
    virtual void LateUpdate() = 0;
    virtual void OnDisable() = 0;
    virtual void OnDestroy() = 0;


    Entity* const entity;


protected:

    Component();

    static MemoryManager* sMemoryManager;
    std::bitset<(Size)ComponentState::NUM_STATES> mState;
    

private:

    // derived component factory

    template<class Derived>
    static Component* CreateFunc() { return Derived::Instantiate(); }
    // static Component* CreateFunc() { return sMemoryManager->New<Derived>(); }

    static Component* Create( std::string_view componentName) {

        U64 componentTypeIdx = SID( componentName.data());
        if ( sConstructors.Find( componentTypeIdx) == sConstructors.End())
        {
            throw std::runtime_error( FormatStr( "Component::Create => Trying to create component of unknown type %s", componentName) );
        }
            
        return sConstructors[ componentTypeIdx]();
    }

    static Map<U64, PCreateFunc> sConstructors;
    static Map<std::string, bool> sComponentTypes;
};



} // Atuin
