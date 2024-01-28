
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
layout (location = 3) out vec3 fragViewPos;
layout (location = 4) out mat3 fragTBN;


// camera data
layout(set = 0, binding = 0) uniform CameraData {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec4 position;
} camera;

// model data
struct ObjectData {
    mat4 transform;
    vec4 sphereBounds;
};
layout(std140, set = 1, binding = 0) readonly buffer ObjectDataBuffer {
    ObjectData objects[];
} objectBuffer;

// instance object indices
layout(set = 1, binding = 1) readonly buffer InstanceBuffer {
    uint objectIdx[];
} instances;


void main() {

    uint index = instances.objectIdx[ gl_InstanceIndex];
    mat4 model = objectBuffer.objects[ index].transform;
    mat4 normalMatrix = transpose( inverse( model));

    vec4 worldPos = model * vec4(vertPosition, 1.0);
    fragPosition = worldPos.xyz;
    fragTexCoord = vertTexCoord;
    fragColor    = vertColor;
    fragViewPos  = (camera.view * worldPos).xyz;

    // calc TBN matrix
    vec3 N = normalize( (normalMatrix * vec4(vertNormal, 0.f)).xyz);
    vec3 T = normalize( (model * vec4(vertTangent, 0.f)).xyz);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross( N, T);
    fragTBN = mat3(T, B, N);

    gl_Position = camera.viewProj * worldPos;
}