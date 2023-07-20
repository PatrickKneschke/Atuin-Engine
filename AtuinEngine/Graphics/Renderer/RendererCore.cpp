
#include "RendererCore.h"
#include "Core/Util/Types.h"

#include "GLFW/glfw3.h"
#include <vector>


// unique definition of default dispatch loader
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE


namespace Atuin {


RendererCore::RendererCore(GLFWwindow *window) : pWindow {window} {

	// default dispatch loader dynamic
	vk::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

	CreateInstance();
#ifndef NDEBUG
    CreateDebugMessenger();
#endif
    CreateSurface();
    ChooseGPU();
    CreateDevice();
}


RendererCore::~RendererCore() {

    // logical device
	if(mDevice)
    {
		mDevice.destroy(nullptr);
	}
    
	// surface
	if(mSurface)
    {
		mInstance.destroySurfaceKHR(mSurface, nullptr);
    }

	// debug messenger
#ifndef NDEBUG
	if(mDebugMessenger)
    {
        mInstance.destroyDebugUtilsMessengerEXT(mDebugMessenger, nullptr);
    }
#endif

	// instance
	if(mInstance)
    {
		mInstance.destroy(nullptr);
    }
}


void RendererCore::CreateInstance() {

    auto appInfo = vk::ApplicationInfo{}
        .setPEngineName("AtuinEngine")
        .setEngineVersion(  VK_MAKE_VERSION(1, 0, 0) )
        .setApiVersion( VK_API_VERSION_1_2 );

    U32 glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> glfwExtensionsVector(glfwExtensions, glfwExtensions + glfwExtensionCount);
    std::vector<const char*> layers;

#ifndef NDEBUG
    glfwExtensionsVector.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    auto instanceInfo = vk::InstanceCreateInfo{}
        .setPApplicationInfo( &appInfo )
        .setEnabledExtensionCount( static_cast<U32>(glfwExtensionsVector.size()) )
        .setPpEnabledExtensionNames( glfwExtensionsVector.data() )
        .setEnabledLayerCount( static_cast<U32>(layers.size()) )
        .setPpEnabledLayerNames( layers.data() );

    vk::Result result = vk::createInstance(&instanceInfo, nullptr, &mInstance);
    if (result != vk::Result::eSuccess) {

       throw std::runtime_error("Failed to create vulkan instance : " + vk::to_string(result));
    }

	// initialize dispatch loader with instance, if creation was successful
	VULKAN_HPP_DEFAULT_DISPATCHER.init(mInstance);
}


void RendererCore::CreateDebugMessenger() {

	auto debugMessengerInfo = vk::DebugUtilsMessengerCreateInfoEXT{}
		.setMessageSeverity(
			// vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError 
		)
		.setMessageType(
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance 
		)
		.setPfnUserCallback(DebugCallback);	
	
	vk::Result result = mInstance.createDebugUtilsMessengerEXT(&debugMessengerInfo, nullptr, &mDebugMessenger);
    if (result != vk::Result::eSuccess)
    {
       throw std::runtime_error("Failed to create debug messenger : " + vk::to_string(result));
    }
}


void RendererCore::CreateSurface() {

	VkSurfaceKHR surface;
	vk::Result result = (vk::Result)glfwCreateWindowSurface(mInstance, pWindow, nullptr, &surface);
    if( result != vk::Result::eSuccess )
    {
        throw std::runtime_error("Failed to create surface : " + vk::to_string(result));
    }
	mSurface = vk::SurfaceKHR(surface);
}


void RendererCore::ChooseGPU() {

    // choose first dedicated gpu found, otherwise choose integrated gpu
    // TODO account for multiple dedicated gpus by scoring and pick best one?
    auto gpus = mInstance.enumeratePhysicalDevices();
	for(auto &gpu : gpus) 
    {	
        auto properties = gpu.getProperties();

        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        {
            mGpu = gpu;
            return;
        }
        else if (properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu) {

            mGpu = gpu;
        }
	}

	if(!mGpu) {
		throw std::runtime_error("Failed to find suitable GPU!");
	}
}


void RendererCore::CreateDevice() {

    // find queue family indices
    auto queueFamilies = mGpu.getQueueFamilyProperties();
    for (Size i = 0; i < queueFamilies.size(); i++)
    {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
        {
            mQueueFamilies.graphicsFamily = (I32)i;
        }
        else if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute)
        {
            mQueueFamilies.computeFamily = (I32)i;
        }
        else if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer)
        {
            mQueueFamilies.transferFamily = (I32)i;
        }
    }
    std::unordered_set<I32> uniqueQueueFamilies = mQueueFamilies.GetUniqueIndices();

    // queue create infos
    std::vector<vk::DeviceQueueCreateInfo> queueInfos;
    float priority = 0.f;
    for (auto family : uniqueQueueFamilies)
    {
        if(family < 0)
        {
            continue;
        }

        auto queueInfo = vk::DeviceQueueCreateInfo{}
            .setPQueuePriorities(&priority)
            .setQueueCount(1)
            .setQueueFamilyIndex(family);

        queueInfos.push_back(queueInfo);
    }
    
    // enable device features
    auto availableFeatures = mGpu.getFeatures();
	auto enabledFeatures = vk::PhysicalDeviceFeatures{};
	if (availableFeatures.samplerAnisotropy)
    {
		enabledFeatures.setSamplerAnisotropy(VK_TRUE);
    }

    // create device
    auto deviceInfo = vk::DeviceCreateInfo{}
        .setQueueCreateInfoCount( (U32)queueInfos.size() )
        .setPQueueCreateInfos( queueInfos.data() )
        .setPEnabledFeatures( &enabledFeatures )
        .setEnabledExtensionCount( (U32)requiredExtensions.size() )
        .setPpEnabledExtensionNames( requiredExtensions.data() );


    vk::Result result = mGpu.createDevice(&deviceInfo, nullptr, &mDevice);
    if (result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to create logical device : " + vk::to_string(result));
    }    

    // init dispatch loader with device
	VULKAN_HPP_DEFAULT_DISPATCHER.init(mDevice);
}

    
} // Atuin
