
#pragma once


#include "MeshPass.h"

#include "vulkan/vulkan.hpp"


namespace Atuin {
    

// used to request a material from the cache
struct MaterialInfo {

    PassData<std::string> pipelineName;
    PassData<Array<std::string>> textureNames;
};


// used for binding the material descriptor and pipeline when changed
struct Material {

    std::string materialName;
    PassData<bool> usedInPass;    
    PassData<U64> pipelineId;
    PassData<vk::DescriptorSet> descriptorSet;
};


struct Texture {

    vk::ImageView imageView;
    vk::Sampler sampler;
};


} // Atuin