
#version 460

// input
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec4 color;

// output
layout (location = 0) out vec3 fragPos;
layout (location = 1) out vec3 fragNormal;
layout (location = 2) out vec2 fragTexCoord;
layout (location = 3) out vec4 fragColor;

// MVP
layout(set = 0, binding = 0) uniform mat4 model;
layout(set = 0, binding = 1) uniform mat4 view;
layout(set = 0, binding = 2) uniform mat4 proj; 


void main() {

    vec4 worldPos     = model * pos;
    mat3 normalMatrix = transpose( invert( mat3(model)));

    fragPos       = worldPos.xyz;
    fragNormal    = normalMatrix + normal;
    fragTexCoord  = texCoord;
    fragColor     = color;

    gl_Position = proj * view * worldPos;
}