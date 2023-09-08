
#pragma once


#include "MeshPass.h"

#include "vulkan/vulkan.hpp"


namespace Atuin {
    

// used for binding the material descriptor and pipeline when changed
struct Material {

    U64 materialId;

    PassData<bool> usedInPass;
    
    PassData<vk::Pipeline> pipeline;
    PassData<vk::PipelineLayout> pipelineLayout;
    PassData<vk::DescriptorSet> descriptorSet;
};


struct Texture {

    vk::ImageView imageView;
    vk::Sampler sampler;
};



} // Atuin