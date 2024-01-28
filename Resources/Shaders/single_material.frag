
#version 460

// input
layout (location = 0) in vec3 fragPosition;
layout (location = 1) in vec4 fragColor;
layout (location = 2) in vec2 fragTexCoord;
layout (location = 3) in mat3 fragTBN;


// output
layout (location = 0) out vec4 outColor;


// camera data
layout(set = 0, binding = 0) uniform CameraData {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec3 position;
} camera;


// scene data
struct AmbientLight {
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    vec3 color; 
    float intensity;
    vec3 direction;
};

layout(set = 0, binding = 1) uniform SceneData {
    AmbientLight ambient;
    DirectionalLight light;
} scene;


// samplers
layout(set = 2, binding = 0) uniform sampler2D diffuseMap;
layout(set = 2, binding = 1) uniform sampler2D normalMap;
layout(set = 2, binding = 2) uniform sampler2D specularMap;


void main() {

    vec3 diffuse = texture(diffuseMap, fragTexCoord).rgb;
    float specular = texture(specularMap, fragTexCoord).r;
    vec3 normal =  normalize( 2.0 * texture(normalMap, fragTexCoord).xyz - 1.0);
    normal = normalize( fragTBN * normal);

    // ambient lighting
    vec3 ambientComp = scene.ambient.intensity * scene.ambient.color * diffuse;

    // diffuse lighting
    vec3 lightDir = normalize( -scene.light.direction);
    float angle = clamp( dot(normal, lightDir), 0.f, 1.f);
    vec3 diffuseComp = scene.light.intensity * scene.light.color * diffuse * angle; 
    
    // specular lighting (Phong)
    vec3 viewDir = normalize( camera.position - fragPosition);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow( clamp(dot(viewDir, reflectDir), 0.f, 1.f), specular);
    vec3 specularComp = scene.light.intensity * scene.light.color * diffuse * spec;

    outColor = fragColor * vec4( ambientComp + diffuseComp + specularComp, 1);    
}