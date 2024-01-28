
#pragma once


#include "Scene/Component.h"

#include "glm/glm.hpp"


namespace Atuin {


DEFINE_COMPONENT( MeshComponent, Component, false)

    friend class Renderer;

public:

    void Init( Json data) override;
    
    void OnEnable() override;
    void Awake() override;
    void Start() override;
    void Update() override;
    void FixedUpdate() override;
    void LateUpdate() override;
    void OnDisable() override;
    void OnDestroy() override;

private:

    glm::mat4 mTransform;
    glm::vec4 mSphereBounds;

    std::string mMeshName;
    std::string mMaterialName;

    U32 mObjectIdx;    


DEFINE_COMPONENT_END( MeshComponent, Component, false)

    
} // Atuin
