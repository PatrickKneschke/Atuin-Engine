
#pragma once


#include "MeshPass.h"

#include "vulkan/vulkan.hpp"


namespace Atuin {
    

// used for binding the material descriptor and pipeline when changed
struct Material {

    PassData<bool> usedInPass;
    
    PassData<vk::Pipeline> pipeline;
    PassData<vk::PipelineLayout> pipelineLayout;
    PassData<vk::DescriptorSet> descriptorSet;
};


} // Atuin