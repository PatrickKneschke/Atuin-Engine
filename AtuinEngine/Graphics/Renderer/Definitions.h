
#pragma once


#include "Core/Util/Types.h"
#include "Core/DataStructures/Array.h"

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <string>
#include <unordered_set>


namespace Atuin {
    

struct QueueFamilyIndices {

    // for graphics, graphics-related compute and gpu-gpu transfer ops
    I32 graphicsFamily = -1;
    // for async compute ops
    I32 computeFamily = -1;
    // for cpu-gpu transfer ops
    I32 transferFamily = -1;

    std::unordered_set<I32> GetUniqueIndices() {

        return {graphicsFamily, computeFamily, transferFamily};
    }
};

// stores swapchain, its images, image views and all data used to create it
struct Swapchain {
	vk::SwapchainKHR     swapchain;
	Array<vk::Image>     images;
	Array<vk::ImageView> imageViews;
	U32                  imageCount;
	vk::Format           imageFormat;
	vk::ColorSpaceKHR    colorSpace;
	vk::PresentModeKHR   presentMode;
	vk::Extent2D         extent;
};


// stores pipeline, pipeline layout and all data used to create it
struct Pipeline {

	vk::Pipeline							 pipeline;
	vk::PipelineLayout						 pipelineLayout;
	Array<vk::PipelineShaderStageCreateInfo> shaderInfos;
	vk::PipelineVertexInputStateCreateInfo	 vertexInputInfo;
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	vk::PipelineViewportStateCreateInfo      viewportInfo;
	vk::PipelineRasterizationStateCreateInfo rasterizerInfo;
	vk::PipelineMultisampleStateCreateInfo 	 multisampleInfo;
	vk::PipelineDepthStencilStateCreateInfo  depthStencilInfo;
	vk::PipelineColorBlendAttachmentState	 colorBlendAttachment;
	vk::PipelineColorBlendStateCreateInfo 	 colorBlendInfo;
    vk::PipelineDynamicStateCreateInfo       dynamicStateInfo;
    vk::RenderPass                           renderpass;
    U32                                      subpass;
};

// stores an image, its device memory, image view and relevant data
struct ImageResource {
	vk::Image               image;
	vk::DeviceMemory        imageMemory;
	vk::ImageView           imageView;
	vk::Format              format;
	vk::ImageUsageFlags     usage;
	vk::MemoryPropertyFlags memoryType;
	U32                     width;
	U32                     height;

	vk::DescriptorImageInfo DescriptorInfo( vk::Sampler sampler) {

		auto imageInfo = vk::DescriptorImageInfo{}
			.setImageLayout( vk::ImageLayout::eShaderReadOnlyOptimal )
			.setImageView( imageView )
			.setSampler( sampler );

		return imageInfo;
	}
};

// stores a buffer, its device memory, usage and memory type
struct Buffer {
	vk::Buffer				buffer;
	vk::DeviceMemory		bufferMemory;
	vk::DeviceSize          bufferSize = 0;
	vk::BufferUsageFlags	usage;
	vk::MemoryPropertyFlags	memoryType;

	vk::DescriptorBufferInfo DescriptorInfo( U32 offset = 0, Size range = VK_WHOLE_SIZE) {

		auto bufferInfo = vk::DescriptorBufferInfo{}
			.setBuffer( buffer )
			.setOffset( offset )
			.setRange( range );

		return bufferInfo;
	}
};


struct CameraData {

	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewProj;
	glm::vec3 position;
};


struct ObjectData {

	glm::mat4 transform;
	// glm::vec4 sphereBounds;
};


struct AmbientLight {

    glm::vec3 color;
    float intensity;
};


struct DirectionalLight {

    glm::vec3 color; 
    float intensity;
    glm::vec3 direction; 
};


struct SceneData {

	AmbientLight ambient;
	DirectionalLight light;
};


struct ImmediateSubmitContext{

	vk::Fence fence;
	vk::CommandPool commandPool;
	vk::CommandBuffer commandBuffer;
};


} // Atuin

