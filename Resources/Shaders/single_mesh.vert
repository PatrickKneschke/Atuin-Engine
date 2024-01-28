
#version 460

// input
layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec4 vertColor;
layout (location = 2) in vec2 vertTexCoord;
layout (location = 3) in vec3 vertNormal;
layout (location = 4) in vec3 vertTangent;

// output
layout (location = 0) out vec3 fragPosition;
layout (location = 1) out vec4 fragColor;
layout (location = 2) out vec2 fragTexCoord;
layout (location = 3) out mat3 fragTBN;


// camera data
layout(set = 0, binding = 0) uniform CameraData {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec3 position;
} camera;

// model data
layout(set = 1, binding = 0) uniform ObjectData{
    mat4 model;
} object;


void main() {

    vec4 worldPos = object.model * vec4(vertPosition, 1.0);
    mat4 normalMatrix = transpose( inverse( object.model));

    fragPosition = worldPos.xyz;
    fragTexCoord = vertTexCoord;
    fragColor    = vertColor;

    // calc TBN matrix
    vec3 N = normalize( (normalMatrix * vec4(vertNormal, 0.f)).xyz);
    vec3 T = normalize( (object.model * vec4(vertTangent, 0.f)).xyz);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross( N, T);
    fragTBN = mat3(T, B, N);
    
    gl_Position = camera.viewProj * worldPos;
}