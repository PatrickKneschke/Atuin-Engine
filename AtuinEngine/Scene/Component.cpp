
#include "Component.h"


namespace Atuin {


Map<U64, Component::PCreateFunc> Component::sConstructors;
Map<std::string, bool> Component::sComponentTypes;


Component::Component() : entity {nullptr} {

    // call Start() after creation and adding to entity
    SetActive( true);
}


bool Component::GetState( ComponentState state) const {

    return mState.test( (Size)state);
}


void Component::SetState( ComponentState state, bool value) {

    mState.set( (Size)state, value);
}


bool Component::IsActive() const {

    return GetState( ComponentState::ACTIVE);
}


void Component::SetActive( bool value) {

    bool isActive = GetState( ComponentState::ACTIVE);
    SetState( ComponentState::ACTIVE, value);
    if ( isActive)
    {
        if( !value  &&  GetState( ComponentState::AWAKE))
        {
            OnDisable();
        }
    }
    else if ( value)
    {
        if ( !GetState( ComponentState::AWAKE))
        {
            Awake();
        }
        OnEnable();
    }
}
    

} // Atuin
