
#include "Entity.h"


namespace Atuin {


Entity* Entity::Instantiate(std::string_view name, Entity* parent) {

    // delegate to scene manager
    return nullptr;
}


Entity::Entity( std::string_view name) : mName { name} {

    // set entity state
    // call onEnable
}


Entity::~Entity() {

    Destroy();
}


void Entity::OnEnable() {

    // call OnEnable() on all cmponents
}


void Entity::OnDisable() {

    // call OnDisable() on all active cmponents
}


void Entity::Destroy() {

    // destroy entity if it needs destruction
    // otherwise destroy all components that need destruction

    // disconnect parent and child transforms
}


void Entity::Update() {

    // call Update() on all active cmponents that need update
}


void Entity::FixedUpdate() {

    // call FixedUpdate() on all active cmponents that need fixed update
}


void Entity::LateUpdate() {

    // call LateUpdate() on all active cmponents taht need late update
}


} // Atuin