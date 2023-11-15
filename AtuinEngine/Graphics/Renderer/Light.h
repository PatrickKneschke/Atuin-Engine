#pragma once 


#include "Definitions.h"


namespace Atuin {


struct Light {

    glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
    float     intensity = 1.f;
};


struct DirectionalLight : Light{

    glm::mat4 viewProj;
    Image shadowMap; // layered for cascading map
    glm::vec3 direction = glm::vec3(0.f, -1.f, 0.f);
};


struct SpotLight : Light{

    glm::mat4 viewProj;
    Image shadowMap; // single layer
    glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 direction = glm::vec3(0.f, -1.f, 0.f);
};


struct PointLight : Light{

    // glm::mat4 viewProj;
    Image shadowMap; // cube map
    glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
};

    
} // Atuin
