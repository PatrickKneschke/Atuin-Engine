
#pragma once


#include "Definitions.h"
#include "Core/Util/Types.h"
#include "Core/DataStructures/Array.h"

#include <vulkan/vulkan.hpp>

#include <iostream>


class GLFWwindow;

namespace Atuin {


// required vulkan extensions
const Array<const char*> requiredExtensions {

	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


class RendererCore {

// TODO use memory allocator -> vma???

public:

    RendererCore() = default;
    RendererCore(GLFWwindow* window);
    ~RendererCore();

    // access functions
    GLFWwindow* Window() const { return pWindow; }
	vk::Instance Instance() const { return mInstance; }
	vk::PhysicalDevice Gpu() const { return mGpu; }
	vk::Device Device() const { return mDevice; }
	vk::SurfaceKHR Surface() const { return mSurface; }
	
	vk::PhysicalDeviceFeatures gpuFeatures() const { return mGpuFeatures; }
	vk::PhysicalDeviceProperties gpuProperties() const { return mGpuProperties; }
	vk::PhysicalDeviceMemoryProperties gpuMemoryProperties() const { return mGpuMemoryProperties; }
	QueueFamilyIndices QueueFamilies() const { return mQueueFamilies; }
    vk::Extent2D SurfaceExtent() const { return mGpu.getSurfaceCapabilitiesKHR(mSurface).currentExtent; }
	
	vk::Queue GraphicsQueue() const { return mGraphicsQueue; }
	vk::Queue ComputeQueue() const { return mComputeQueue; }
	vk::Queue TransferQueue() const { return mTransferQueue; }

    // creation functions
    vk::Buffer CreateBuffer( 
        vk::DeviceSize size, 
        vk::BufferUsageFlags usage,  
        vk::SharingMode sharingMode = vk::SharingMode::eExclusive,
        U32 queueFamilyCount = 0,
        const U32* pQueueFamilies = nullptr
    ) const;
    vk::DeviceMemory AllocateBufferMemory( 
        vk::Buffer buffer, 
        vk::MemoryPropertyFlags properties
    ) const;
    vk::Image CreateImage(
		U32 width,
		U32 height,
		vk::Format format,
		vk::ImageUsageFlags usage,
		U32 mipLevels = 1,
		vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
		vk::SampleCountFlagBits	numSamples = vk::SampleCountFlagBits::e1,
        vk::SharingMode = vk::SharingMode::eExclusive,
        U32 queueFamilyCount = 0,
        const U32* pQueueFamilies = nullptr
	) const;
	vk::DeviceMemory AllocateImageMemory(
		vk::Image image,
		vk::MemoryPropertyFlags properties
	) const;
	vk::ImageView CreateImageView(
		vk::Image image, vk::Format format,
		vk::ImageAspectFlags aspectFlags,
		U32 mipLevels = 1
	) const;
    vk::Sampler CreateSampler(
        vk::Filter magFilter,
        vk::Filter minFilter,
		vk::SamplerMipmapMode mipmapMode,
		vk::SamplerAddressMode addressModeU,
		vk::SamplerAddressMode addressModeV,
		vk::SamplerAddressMode addressModeW,
		bool enableAnisotropy,
        float minLod = 0.f,
        float maxLod = 1.f,
        float mipLodBias = 0.f,
		bool unnormalized = VK_FALSE,
		bool enableCompare = VK_FALSE,
        vk::CompareOp compareOp = vk::CompareOp::eAlways,
		vk::BorderColor borderColor = vk::BorderColor::eFloatOpaqueBlack
    ) const;
    vk::CommandPool CreateCommandPool(
        U32 queueFamily,
		vk::CommandPoolCreateFlags flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer
    ) const;
    vk::CommandBuffer AllocateCommandBuffer(
        vk::CommandPool commandPool,
        vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary
    ) const;
    Array<vk::CommandBuffer> AllocateCommandBuffers(
        vk::CommandPool commandPool,
        U32 count,
        vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary
    ) const;
	vk::Fence CreateFence( vk::FenceCreateFlags signaled = vk::FenceCreateFlags{} ) const;	
	vk::Semaphore CreateSemaphore() const;
    vk::ShaderModule CreateShaderModule( Size codeSize, const char* code ) const;
    vk::DescriptorPool CreateDescriptorPool(
        U32 maxSets,
        U32 count, 
        vk::DescriptorPoolSize* poolSizes,
        vk::DescriptorPoolCreateFlags flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet
    ) const;
    vk::DescriptorSetLayout CreateDescriptorSetLayout( U32 count, vk::DescriptorSetLayoutBinding* bindings ) const;
    vk::DescriptorSet AllocateDescriptorSet( vk::DescriptorPool pool, vk::DescriptorSetLayout layout) const;
    Array<vk::DescriptorSet> AllocateDescriptorSets( vk::DescriptorPool pool, U32 count, vk::DescriptorSetLayout* layouts ) const;
    vk::PipelineLayout CreatePipelineLayout(
        U32 setLayoutCount,
        vk::DescriptorSetLayout* setLayouts,
        U32 pushConstantCount = 0,
        vk::PushConstantRange* pushConstants = nullptr
    ) const;
    vk::RenderPass CreateRenderPass(
        U32 attachmentCount,
        vk::AttachmentDescription* attachments,
        U32 subpassCount,
        vk::SubpassDescription* subpasses,
        U32 dependencyCount = 0,
        vk::SubpassDependency* dependencies = nullptr
    ) const;
    vk::Framebuffer createFrameBuffer(
        vk::RenderPass renderPass,
        U32 attachmentCount,
        vk::ImageView* attachments,
        U32 width,
        U32 height,
        U32 layers = 1
    ) const;
    void CreatePipeline( Pipeline &pipeline ) const;
    void PrepareSwapchain( Swapchain &swapchain ) const; 
    void CreateSwapchain( Swapchain &swapcahin ) const;
    

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void*  )  
    {
        std::cerr << "validation layer  [" <<  messageSeverity << "], (" << messageType << ") : " << pCallbackData->pMessage << '\n';

        return VK_FALSE;
    }
        

private:

	void CreateInstance();
	void CreateDebugMessenger();
	void CreateSurface();
	void ChooseGPU();
    void CreateDevice();
    void CreateQueues();


    GLFWwindow* pWindow;

    vk::Instance mInstance;
    vk::DebugUtilsMessengerEXT mDebugMessenger;
    vk::SurfaceKHR mSurface;

    vk::PhysicalDevice mGpu;
    vk::PhysicalDeviceFeatures mGpuFeatures;
    vk::PhysicalDeviceProperties mGpuProperties;
    vk::PhysicalDeviceMemoryProperties mGpuMemoryProperties;
    QueueFamilyIndices mQueueFamilies;
    
    vk::Device mDevice;
    vk::Queue mGraphicsQueue; // for graphics, graphics-related compute and gpu-gpu transfer ops
    vk::Queue mComputeQueue;  // for async compute ops
    vk::Queue mTransferQueue; // for cpu-gpu transfer ops

};


} // Atuin
