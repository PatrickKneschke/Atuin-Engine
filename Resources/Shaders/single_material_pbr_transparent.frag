
#version 460

// input
layout (location = 0) in vec3 fragPosition;
layout (location = 1) in vec4 fragColor;
layout (location = 2) in vec2 fragTexCoord;
layout (location = 3) in vec3 fragViewPos;
layout (location = 4) in mat3 fragTBN;


// output
layout (location = 0) out vec4 outColor;


// camera data
layout(set = 0, binding = 0) uniform CameraData {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec4 position;
} camera;

// scene data
layout(set = 0, binding = 1) uniform SceneData {
    vec3 ambientColor;
    float ambientIntensity;
    vec3 sunColor; 
    float sunIntensity;
    vec3 sunDirection;
} scene;

// light view proj matrices
layout(set = 0, binding = 2) readonly buffer LightData {
    mat4 viewProj[];
} light;

// shadow sampler
layout(set = 0, binding = 3) uniform sampler2DArrayShadow shadowMap;

// samplers
layout(set = 2, binding = 0) uniform sampler2D albedoMap;
layout(set = 2, binding = 1) uniform sampler2D normalMap;
layout(set = 2, binding = 2) uniform sampler2D metallicMap;
layout(set = 2, binding = 3) uniform sampler2D roughnessMap;
layout(set = 2, binding = 4) uniform sampler2D aoMap;
layout(set = 2, binding = 5) uniform sampler2D opacityMap;

// cascade count
layout(push_constant) uniform Constant {
    uint cascadeCount;
};


const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
//                     PCR Shadow Calculation
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 normal, vec3 halfway, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float cosH = max( dot(normal, halfway), 0.0);
    float cosH2 = cosH * cosH;

    float nom   = a2;
    float denom = (cosH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float cosV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = cosV;
    float denom = cosV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
    float cosV = max(dot(normal, viewDir), 0.0);
    float cosL = max(dot(normal, lightDir), 0.0);
    float ggx2 = GeometrySchlickGGX(cosV, roughness);
    float ggx1 = GeometrySchlickGGX(cosL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
//                     PCR Shadow Calculation
// ----------------------------------------------------------------------------
float calcShadow( vec4 pos, vec2 offset, uint cascadeIdx)
{
    vec4 texCoord = pos / pos.w;
    texCoord.xy = texCoord.st * 0.5 + 0.5;
    float shadow = texture( shadowMap, vec4(texCoord.st + offset, cascadeIdx, texCoord.z));

    return shadow;
}

float shadowPCF(vec4 pos, uint cascadeIdx)
{
	ivec2 texDim = textureSize(shadowMap, 0).xy;
	float scale = max(1.0, 3.0 - cascadeIdx);
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += calcShadow(pos, vec2(dx*x, dy*y), cascadeIdx);
			count++;
		}	
	}

	return shadowFactor / count;
}


void main() {

    vec3 albedo     = texture(albedoMap, fragTexCoord).rgb;
    // vec3 albedo     = pow( texture(albedoMap, fragTexCoord).rgb, vec3(2.2f));
    vec3 normal     = normalize( fragTBN * (texture(normalMap, fragTexCoord).xyz * 2.0 - 1.0));
    float metallic  = texture(metallicMap, fragTexCoord).r;
    float roughness = texture(roughnessMap, fragTexCoord).r;
    float ao        = texture(aoMap, fragTexCoord).r;
    float opacity   = texture(opacityMap, fragTexCoord).r;

     
    // calc ambient component
    vec3 ambient = scene.ambientIntensity * scene.ambientColor * albedo * ao;
    vec3 color = fragColor.rgb * ambient;

    // calc shadow
    uint cascadeIdx = 0;
    for (uint i = 0; i < cascadeCount - 1; i++)
    {
        float cascadeSplit = light.viewProj[i][3][3];
        if ( fragViewPos.z < cascadeSplit)
        {
            cascadeIdx = i + 1;
        }
    }
    mat4 shadowViewProj = light.viewProj[ cascadeIdx];
    shadowViewProj[3][3] = 1.0; 
    vec4 shadowCoord = shadowViewProj * vec4(fragPosition, 1);
    float shadow = shadowPCF( shadowCoord, cascadeIdx);

    // calc diffuse component only if a relevant amount of light hits the frag
    if ( shadow > scene.ambientIntensity)
    {
        // diffuse component based on metalicity
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);
                
        // calculate light radiance
        vec3 viewDir = normalize(camera.position.xyz - fragPosition);
        vec3 lightDir = normalize( -scene.sunDirection);
        vec3 halfway = normalize(viewDir + lightDir);
        
        vec3 radiance = scene.sunColor * scene.sunIntensity;
            
        // cook-torrance brdf
        float NDF = DistributionGGX( normal, halfway, roughness);       
        float G = GeometrySmith( normal, viewDir, lightDir, roughness);
        vec3 F = FresnelSchlick( max(dot(halfway, viewDir), 0.0), F0);
            
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
            
        // reflectance equation   
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
                
        // calc outgoing radiance L0
        float cosL = max(dot(normal, lightDir), 0.0);
        vec3 L0 = (kD * albedo / PI + specular) * radiance * cosL;

        // finalcolor
        color = fragColor.rgb * (ambient + shadow * L0);
        
        // // gamma correction
        // color = color / (color + vec3(1.0));
        // color = pow(color, vec3(1.0f / 2.2f)); 
    }
   
    outColor = vec4( color, opacity);
}