
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

	//TODO craete DescriptorImageInfo
};

// stores a buffer, its device memory, usage and memory type
struct Buffer {
	vk::Buffer				buffer;
	vk::DeviceMemory		bufferMemory;
	vk::DeviceSize          bufferSize;
	vk::BufferUsageFlags	usage;
	vk::MemoryPropertyFlags	memoryType;

	//TODO create DescriptorBufferInfo
};

// stores all data of a single mesh vertex
struct Vertex {

	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoord;
	glm::vec3 normal;
	glm::vec3 tangent;

	static vk::VertexInputBindingDescription getBindingDescription() {
	
		auto bindingDescription = vk::VertexInputBindingDescription{}
			.setBinding( 0 )
			.setStride( sizeof(Vertex) )
			.setInputRate( vk::VertexInputRate::eVertex );
		
		return bindingDescription;
	}
	
	static Array<vk::VertexInputAttributeDescription> getAttributeDescriptions() {

		Array<vk::VertexInputAttributeDescription> attributeDescriptions;
		attributeDescriptions.Resize(5);
		
		attributeDescriptions[0]
			.setLocation( 0 )
			.setBinding( 0 )
			.setFormat( vk::Format::eR32G32B32Sfloat )
			.setOffset( offsetof(Vertex, position) );
		attributeDescriptions[1]
			.setLocation( 1 )
			.setBinding( 0 )
			.setFormat( vk::Format::eR32G32B32A32Sfloat )
			.setOffset( offsetof(Vertex, color) );
		attributeDescriptions[2]
			.setLocation( 2 )
			.setBinding( 0 )
			.setFormat( vk::Format::eR32G32Sfloat )
			.setOffset( offsetof(Vertex, texCoord) );
		attributeDescriptions[3]
			.setLocation( 3 )
			.setBinding( 0 )
			.setFormat( vk::Format::eR32G32B32Sfloat )
			.setOffset( offsetof(Vertex, normal) );
		attributeDescriptions[4]
			.setLocation( 4 )
			.setBinding( 0 )
			.setFormat( vk::Format::eR32G32B32Sfloat )
			.setOffset( offsetof(Vertex, tangent) );
		
		return attributeDescriptions;
	}
	
	bool operator ==(const Vertex &other) const {
		
		return position == other.position && color == other.color && texCoord == other.texCoord
				&& normal == other.normal && tangent == other.tangent;
	}
};


struct CameraData {

	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewProj;
	glm::vec3 position;
};


struct ObjectData {

	glm::mat4 model;
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


struct FrameResources {

	vk::Fence renderFence;
	vk::Semaphore renderSemaphore;
	vk::Semaphore presentSemaphore;

	vk::CommandPool commandPool;
	vk::CommandBuffer commandBuffer;
};


struct ImmediateSubmitContext{

	vk::Fence fence;
	vk::CommandPool commandPool;
	vk::CommandBuffer commandBuffer;
};


} // Atuin


// custom hash for Vertex type
namespace std {
	template<> 
	struct hash<Atuin::Vertex> {
			
		size_t operator()(Atuin::Vertex const &vertex) const {
				
			return ((((hash<glm::vec3>()(vertex.position) ^
				     (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
					 (hash<glm::vec2>()(vertex.texCoord) << 1) >> 1) ^
					 (hash<glm::vec3>()(vertex.normal) << 1) >> 1) ^
					 (hash<glm::vec3>()(vertex.tangent) << 1);
		}
	};
}
