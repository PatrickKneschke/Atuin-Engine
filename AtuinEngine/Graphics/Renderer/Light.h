#pragma once 


#include "Definitions.h"
#include "Core/DataStructures/Array.h"


namespace Atuin {


struct ShadowCascade {
    
	glm::mat4 view;
    glm::mat4 proj;
	float splitDepth;

	vk::Framebuffer framebuffer;
	vk::ImageView imageView;
};


struct Light {

    glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
    float     intensity = 1.f;
};


struct DirectionalLight : Light{

    glm::vec3 direction = glm::vec3(0.f, -1.f, 0.f);
    Image shadowImage; // layered for cascading map
    Array<ShadowCascade> cascades;
    Buffer cascadeViewBuffer;
    Buffer cascadeViewProjBuffer;
};


struct SpotLight : Light{

    glm::mat4 viewProj;
    Image shadowImage; // single layer
    glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 direction = glm::vec3(0.f, -1.f, 0.f);
};


struct PointLight : Light{

    // glm::mat4 viewProj;
    Image shadowImage; // cube map
    glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
};

    
} // Atuin
