
#include "Camera.h"


namespace Atuin {


glm::mat4 Camera::View() {

    glm::mat4 view = glm::lookAt(
		position,
		center,
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
