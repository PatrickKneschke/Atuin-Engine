
#pragma once


#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>


namespace Atuin {


struct Camera {

    glm::vec3 position = glm::vec3( 0.f, 0.f, 10.f);
    // glm::vec3 rotation = glm::vec3( 0.f, 0.f, 0.f);
    
    glm::vec3 forward = glm::vec3( 0.f, 0.f, 1.f);
    glm::vec3 up = glm::vec3( 0.f, 1.f, 0.f);
    glm::vec3 right = glm::vec3( 1.f, 0.f, 0.f);

    float fov = glm::radians( 60.f);
    float zoom = 1.f;
    float aspect = 1.f;
    float zNear = 0.1f;
    float zFar = 1000.f;
    bool ortho = false;


    void MoveTo( glm::vec3 newPosition);
    void Pan( glm::vec3 pan);

    void Rotate( float angle, glm::vec3 axis);
    void Pitch( float angle);
    void Yaw( float angle);
    void Roll( float angle);
    
    void UpdateCoordinates();

    glm::mat4 View();
    glm::mat4 Projection();

};

    
} // Atuin
