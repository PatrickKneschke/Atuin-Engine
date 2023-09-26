
#pragma once


#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>


namespace Atuin {


struct Camera {

    glm::vec3 position = glm::vec3( 10.f, 0.f, 0.f);
    glm::vec3 center = glm::vec3( 0.f);;
    glm::vec3 up = glm::vec3( 0.f, 1.f, 0.f);;
    float fov = glm::radians( 60.f);
    float zoom = 1.f;
    float aspect = 1.f;
    float zNear = 0.1f;
    float zFar = 1000.f;
    bool ortho = false;

    glm::mat4 View();
    glm::mat4 Projection();

};

    
} // Atuin
