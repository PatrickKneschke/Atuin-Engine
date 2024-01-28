
#version 460

// input
layout (location = 0) in vec3 fragPos;
layout (location = 1) in vec3 fragNormal;
layout (location = 2) in vec2 fragTexCoord;
layout (location = 3) in vec4 fragColor;

// output
layout (location = 0) out vec3 gPosition
layout (location = 1) out vec3 gNormal
layout (location = 2) out vec4 gAlbedoSpec

// samplers
layout(set = 1, binding = 0) uniform sampler2D diffuseMap;
layout(set = 1, binding = 1) uniform sampler2D normalMap;

void main() {


}