
#pragma once


#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#include <iostream>


class GLFWwindow;

namespace Atuin {

// required vulkan extensions
const std::vector<const char*> requiredExtensions {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*  )  
{
    std::cerr << "validation layer  [" <<  messageSeverity << "], (" << messageType << ") : " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
    

class RendererCore {

public:

    RendererCore() = default;
    RendererCore(GLFWwindow* window);
    ~RendererCore();


private:

	void CreateInstance();
	void CreateDebugMessenger();
	void CreateSurface();
	void ChooseGPU();


    GLFWwindow*                 pWindow;

    vk::Instance                mInstance;
    vk::DebugUtilsMessengerEXT  mDebugMessenger;
    vk::SurfaceKHR              mSurface;
    vk::PhysicalDevice          mGpu;
};


} // Atuin
