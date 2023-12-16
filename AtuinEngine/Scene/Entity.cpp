
#include "Entity.h"
#include "Component.h"
#include "Transform.h"


namespace Atuin {


Entity* Entity::Instantiate(std::string_view name, Entity* parent) {

    // delegate to scene ?
    return nullptr;
}


void Entity::Destroy( Entity *entity) {

    entity->SetState( EntityState::DESTROY, true);
    for ( auto child : entity->transform->GetChildren())
    {
        Destroy( child->entity);
    }

    entity->transform->SetParent( nullptr);
    entity->transform->RemoveAllChildren();
}


Entity::Entity( std::string_view name) : mName {name}, transform {nullptr} {

    OnEnable();
}


Entity::~Entity() {

    OnDestroy();
}


bool Entity::GetState( EntityState state) const {

    mState.test( (Size)state);
}


void Entity::SetState( EntityState state, bool value) {

    mState.set( (Size)state, value);
}

    
bool Entity::IsActive() const {

    return GetState( EntityState::ACTIVE);
}


void Entity::SetActive( bool value) {

    bool isActive = GetState( EntityState::ACTIVE);
    if ( !isActive  &&  value )
    {
        OnEnable();
    }
    else if ( isActive  &&  !value)
    {
        OnDisable();
    }

}


void Entity::OnEnable() {

    for ( auto &[typeIdx, components] : mComponents)
    {
        for ( auto component : components)
        {
            component->OnEnable();
        }
    }
}


void Entity::OnDisable() {

    for ( auto &[typeIdx, components] : mComponents)
    {
        for ( auto component : components)
        {
            component->OnDisable();
        }
    }
}


void Entity::OnDestroy() {

    
    for ( auto &[typeIdx, components] : mComponents)
    {
        for ( auto component : components)
        {
            component->OnDestroy();
        }
    }

    mComponents.Clear();
}


void Entity::Update() {
    
    for ( auto &[typeIdx, components] : mComponents)
    {
        for ( auto component : components)
        {
            if ( component->IsActive())
            {
                component->Update();
            }
        }
    }
}


void Entity::FixedUpdate() {

    for ( auto &[typeIdx, components] : mComponents)
    {
        for ( auto component : components)
        {
            if ( component->IsActive())
            {
                component->FixedUpdate();
            }
        }
    }
}


void Entity::LateUpdate() {

    for ( auto &[typeIdx, components] : mComponents)
    {
        for ( auto component : components)
        {
            if ( component->IsActive())
            {
                component->LateUpdate();
            }
        }
    }
}


} // Atuin