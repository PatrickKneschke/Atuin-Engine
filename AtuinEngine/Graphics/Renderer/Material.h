
#pragma once


#include "VulkanInclude.h"
#include "MeshPass.h"


namespace Atuin {
    

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