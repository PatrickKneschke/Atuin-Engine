
#version 460

// input
layout (location = 0) in vec2 fragTexCoord;

// output
layout (location = 0) out vec4 outColor;

// sampler
layout (set = 0, binding = 0) uniform sampler2D image;


void main() {

    outColor = vec4( vec3( texture( image, fragTexCoord).x), 1.f);
}