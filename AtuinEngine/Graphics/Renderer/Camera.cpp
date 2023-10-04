
#include "Camera.h"


namespace Atuin {


void Camera::UpdateCoordinates() {

	right = glm::normalize( glm::cross( up, -forward));
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
