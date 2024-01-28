
#version 460

// input
layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec4 vertColor;
layout (location = 2) in vec2 vertTexCoord;
layout (location = 3) in vec3 vertNormal;
layout (location = 4) in vec3 vertTangent;


// camera data
layout(set = 0, binding = 0) readonly buffer LightData {
     mat4 viewProj[];
} light;

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

layout(push_constant) uniform Constant {
    uint cascadeIdx;
};


void main() {

    uint index = instances.objectIdx[ gl_InstanceIndex];
    mat4 model = objectBuffer.objects[ index].transform;
    mat4 normalMatrix = transpose( inverse( model));

    vec4 worldPos = model * vec4(vertPosition, 1.0);

    mat4 viewProj = light.viewProj[ cascadeIdx];
    viewProj[3][3] = 1.0; 
    gl_Position = viewProj * worldPos;
}