
#pragma once


#include "Core/Util/Types.h"
#include "Core/DataStructures/Array.h"

#include <vulkan/vulkan.hpp>
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
};

// stores a buffer, its device memory, usage and memory type
struct Buffer {
	vk::Buffer				buffer;
	vk::DeviceMemory		bufferMemory;
	vk::BufferUsageFlags	usage;
	vk::MemoryPropertyFlags	memoryType;
};


} // Atuin
