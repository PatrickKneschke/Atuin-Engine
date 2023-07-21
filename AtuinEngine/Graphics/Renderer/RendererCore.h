
#pragma once


#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#include "Definitions.h"

#include <iostream>


class GLFWwindow;

namespace Atuin {


// required vulkan extensions
const std::vector<const char*> requiredExtensions {

	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


class RendererCore {

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
    vk::Extent2D surfaceExtent() const { return mGpu.getSurfaceCapabilitiesKHR(mSurface).currentExtent; }
	
	vk::Queue GraphicsQueue() const { return mGraphicsQueue; }
	vk::Queue ComputeQueue() const { return mComputeQueue; }
	vk::Queue TransferQueue() const { return mTransferQueue; }

    // creation functions
    vk::Buffer CreateBuffer( 
        vk::DeviceSize size, 
        vk::BufferUsageFlags usage,  
        vk::SharingMode sharingMode = vk::SharingMode::eExclusive,
        uint32_t queueFamilyCount = 0,
        const uint32_t* pQueueFamilies = nullptr
    ) const;
    vk::DeviceMemory AllocateBufferMemory( 
                vk::Buffer buffer, 
                vk::MemoryPropertyFlags properties
    ) const;
    vk::Image CreateImage(
		uint32_t width,
		uint32_t height,
		vk::Format format,
		vk::ImageUsageFlags usage,
		uint32_t mipLevels 	= 1,
		vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
		vk::SampleCountFlagBits	numSamples = vk::SampleCountFlagBits::e1,
        vk::SharingMode = vk::SharingMode::eExclusive,
        uint32_t queueFamilyCount = 0,
        const uint32_t* pQueueFamilies = nullptr
	) const;
	vk::DeviceMemory allocateImageMemory(
		vk::Image image,
		vk::MemoryPropertyFlags properties
	) const;
	vk::ImageView createImageView(
		vk::Image image, vk::Format format,
		vk::ImageAspectFlags aspectFlags,
		uint32_t mipLevels = 1
	) const;
    

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void*  )  
    {
        std::cerr << "validation layer  [" <<  messageSeverity << "], (" << messageType << ") : " << pCallbackData->pMessage << std::endl;

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
