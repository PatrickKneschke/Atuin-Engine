
#pragma once


#include "VulkanInclude.h"
#include "Core/Util/Types.h"
#include "Core/DataStructures/Array.h"
#include "Core/DataStructures/Json.h"



namespace Atuin {


void CreatePipelineLayout( vk::Device device, const Array<vk::DescriptorSetLayout> &descriptorLayouts, vk::PipelineLayout *pipelineLayout);


struct Pipeline {

	enum class Type : U8 {

		GRAPHICS = 0,
		COMPUTE
	};

	Type type;
    vk::Pipeline pipeline;
    vk::PipelineLayout pipelineLayout;
};


struct GraphicsPipelineBuilder {

	Array<vk::DescriptorSetLayout>             descriptorLayouts;
	Array<vk::PipelineShaderStageCreateInfo>   shaderInfos;
	vk::VertexInputBindingDescription          vertexInputBinding;
	Array<vk::VertexInputAttributeDescription> vertexAttributes;
	vk::PipelineVertexInputStateCreateInfo	   vertexInputInfo;
	vk::PipelineTessellationStateCreateInfo    tesselationInfo;
	vk::PipelineInputAssemblyStateCreateInfo   inputAssemblyInfo;
	vk::PipelineViewportStateCreateInfo        viewportInfo;
	vk::PipelineRasterizationStateCreateInfo   rasterizerInfo;
	vk::PipelineMultisampleStateCreateInfo 	   multisampleInfo;
	vk::PipelineDepthStencilStateCreateInfo    depthStencilInfo;
	vk::PipelineColorBlendAttachmentState	   colorBlendAttachment;
	vk::PipelineColorBlendStateCreateInfo 	   colorBlendInfo;
	Array<vk::DynamicState>                    dynamicStates;
	vk::PipelineDynamicStateCreateInfo         dynamicStateInfo;

	GraphicsPipelineBuilder() : descriptorLayouts {}, shaderInfos {} {}
    
    Pipeline Build( vk::Device device, vk::RenderPass renderpass);
	void FillFromJson( Json &pipelineJson);
};


struct ComputePipelineBuilder {
	
	Array<vk::DescriptorSetLayout>    descriptorLayouts;
	vk::PipelineShaderStageCreateInfo shaderInfo;

	ComputePipelineBuilder() : descriptorLayouts {} {}

    Pipeline Build( vk::Device device);
};

    
} // Atuin
