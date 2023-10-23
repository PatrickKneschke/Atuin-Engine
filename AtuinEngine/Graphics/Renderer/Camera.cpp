
#include "Camera.h"


namespace Atuin {


void Camera::MoveTo( glm::vec3 newPosition) {

	position = newPosition;
}
    

void Camera::Pan( glm::vec3 pan) {

	position += pan;
}


void Camera::Rotate( float angle, glm::vec3 axis) {

	auto rotation = glm::mat3( glm::rotate( glm::mat4(1.f), glm::radians( angle), axis));
	forward = rotation * forward;
	up = rotation * up;
	right = rotation * right;
}


void Camera::Pitch( float angle) {

	Rotate( angle, right);
}
    

void Camera::Yaw( float angle) {

	Rotate( angle, up);
}


void Camera::Roll( float angle) {

	Rotate( angle, forward);
}


void Camera::UpdateCoordinates() {

	forward = glm::normalize( forward);
	right = glm::normalize( glm::cross( glm::vec3(0.f, 1.f, 0.f), -forward));
	up = glm::normalize( glm::cross( -forward, right));
}


glm::mat4 Camera::View() {

    glm::mat4 view = glm::lookAt(
		position,
		position + forward,
		up
	);

    return view;
}


glm::mat4 Camera::Projection() {

    glm::mat4 proj = glm::perspective(
		fov / zoom,
		aspect,
		zNear, 
        zFar
	);
    proj[1][1] *= -1; // flip y scaling factor -> correction of OpenGL inverse y-axis

    return proj;
}

    
} // Atuin 
