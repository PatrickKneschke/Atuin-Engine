
#pragma once


#include "Core/Util/Types.h"
#include "Core/Util/StringID.h"
#include "Core/Util/StringFormat.h"
#include "Core/DataStructures/Map.h"
#include "Core/DataStructures/Json.h"

#include <bitset>
#include <string>


#define DEFINE_COMPONENT(NAME, BASE, UNIQUE)                                    \
    template<bool Unique>                                                       \
    class ComponentRegistry<class NAME, BASE, Unique> {                         \
        protected:                                                              \
            static bool NAME##Registered;                                       \
    };                                                                          \
    class NAME : public BASE, public ComponentRegistry<NAME, BASE, UNIQUE> {    \
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

    friend class Scene;


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

        std::cout << "registered " << componentName << '\n';

        U64 componentTypeIdx = SID(componentName.c_str());
        // add component constructor
        sConstructors[ componentTypeIdx] = &CreateFunc<Derived>;

        // add to component type dict
        auto &componentTypes = GetComponentTypes();
        componentTypes[ componentTypeIdx] = isUnique;

        return true;
    }

    static Map<U64, bool>& GetComponentTypes() {

        static Map<U64, bool> componentTypes;

        return componentTypes;
    }


    virtual ~Component() = default;

    bool IsActive() const;
    void SetActive( bool value);

    bool GetState( ComponentState state) const;
    void SetState( ComponentState state, bool value);

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

    std::bitset<(Size)ComponentState::NUM_STATES> mState;

private:

    // derived component factory

    template<class Derived>
    static Component* CreateFunc() { return new Derived(); }


    template<class Derived>
    static Component* Create( std::string_view componentName) {

        U64 componentTypeIdx = SID( componentName.data());
        if ( sConstructors.Find( componentTypeIdx) == sConstructors.End())
        {
            throw std::runtime_error( FormatStr( "Trying to create component of unknown type %s", componentName) );
        }
            
        return sConstructors[ componentTypeIdx]();
    }

    static Map<U64, PCreateFunc> sConstructors;
};


DEFINE_COMPONENT(TestComponent, Component, true)

public:

    TestComponent() = default;
    ~TestComponent() = default;

    void Init( Json data) {}

    void OnEnable() {}
    void Awake() {}
    void Start() {}
    void Update() {}
    void FixedUpdate() {}
    void LateUpdate() {}
    void OnDisable() {}
    void OnDestroy() {}


DEFINE_COMPONENT_END(TestComponent, Component, true)

    
} // Atuin
