
#include "MeshComponent.h"


namespace Atuin {


Component* MeshComponent::Instantiate() {

    return sMemoryManager->New<MeshComponent>();
}


void MeshComponent::Init( Json data) {

    mMeshName = data.At( "Mesh").ToString();
    mMaterialName = data.At( "Material").ToString();
}


void MeshComponent::OnEnable() {

}


void MeshComponent::Awake() {

}


void MeshComponent::Start() {

}


void MeshComponent::Update() {

}


void MeshComponent::FixedUpdate() {

}


void MeshComponent::LateUpdate() {

}


void MeshComponent::OnDisable() {

}


void MeshComponent::OnDestroy() {

}



    
} // Atuin
