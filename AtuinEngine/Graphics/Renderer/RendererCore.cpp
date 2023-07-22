
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
    if (result != vk::Result::eSuccess) 
    {
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
            break;
        }
        else if (properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
        {
            mGpu = gpu;
        }
	}

	if(!mGpu)
    {
		throw std::runtime_error("Failed to find suitable GPU!");
	}

    mGpuFeatures = mGpu.getFeatures();
    mGpuProperties = mGpu.getProperties();
    mGpuMemoryProperties = mGpu.getMemoryProperties();
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


void RendererCore::CreateQueues() {

    mDevice.getQueue(mQueueFamilies.graphicsFamily, 0, &mGraphicsQueue);
    if(mQueueFamilies.computeFamily >= 0)
    {
        mDevice.getQueue(mQueueFamilies.computeFamily, 0, &mComputeQueue);
    }
    if(mQueueFamilies.transferFamily >= 0)
    {
        mDevice.getQueue(mQueueFamilies.transferFamily, 0, &mTransferQueue);
    }
}


vk::Buffer RendererCore::CreateBuffer( 
        vk::DeviceSize size, 
        vk::BufferUsageFlags usage,  
        vk::SharingMode sharingMode,
        U32 queueFamilyCount,
        const U32* pQueueFamilies ) const
{
    auto bufferInfo = vk::BufferCreateInfo{}
        .setSize( size )
        .setUsage( usage )
        .setSharingMode( sharingMode )
        .setQueueFamilyIndexCount( queueFamilyCount )
        .setPQueueFamilyIndices( pQueueFamilies );

	vk::Buffer buffer;
	vk::Result result = mDevice.createBuffer(&bufferInfo, nullptr, &buffer);
	if(result != vk::Result:: eSuccess)
    {
		throw std::runtime_error("Failed to create buffer " + vk::to_string(result));
	}
	
	return buffer;	
}


vk::DeviceMemory RendererCore::AllocateBufferMemory(
    vk::Buffer buffer, 
    vk::MemoryPropertyFlags properties ) const
{
	auto memoryRequirements = mDevice.getBufferMemoryRequirements(buffer);
	U32 memoryTypeIndex;
	bool memoryTypeFound = false;

	for (U32 i = 0; i < mGpuMemoryProperties.memoryTypeCount; i++)
    {
		if( (memoryRequirements.memoryTypeBits & (1<<i)) && 
			(mGpuMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) 
		{
			memoryTypeIndex = i;
			memoryTypeFound = true;
			break;
		}
	}
	if( !memoryTypeFound ) 
    {
		throw std::runtime_error("Failed to find memory type" );
	}
	
	auto allocInfo = vk::MemoryAllocateInfo{}
		.setAllocationSize( memoryRequirements.size )
		.setMemoryTypeIndex( memoryTypeIndex );
		
	vk::DeviceMemory bufferMemory;
	vk::Result result = mDevice.allocateMemory(&allocInfo, nullptr, &bufferMemory);
	if(result != vk::Result::eSuccess)
    {
		throw std::runtime_error("Failed to allocate image memory : "+ vk::to_string(result));
	}
	
	mDevice.bindBufferMemory(buffer, bufferMemory, 0);
	
	return bufferMemory;
}


vk::Image RendererCore::CreateImage(
	U32 width,
	U32 height,
	vk::Format format,
	vk::ImageUsageFlags usage,
	U32 mipLevels,
	vk::ImageTiling tiling,
	vk::SampleCountFlagBits	numSamples,
    vk::SharingMode sharingMode,
    U32 queueFamilyCount,
    const U32* pQueueFamilies ) const
{
	auto imageInfo = vk::ImageCreateInfo{}
		.setImageType( vk::ImageType::e2D )
		.setExtent( {width, height, 1} )
		.setFormat( format )
		.setUsage( usage )
		.setMipLevels( mipLevels )
		.setArrayLayers( 1 )
		.setTiling( tiling )
		.setSamples( numSamples )
		.setSharingMode( sharingMode )
        .setQueueFamilyIndexCount( queueFamilyCount )
        .setPQueueFamilyIndices( pQueueFamilies )
		.setInitialLayout( vk::ImageLayout::eUndefined );
		
	vk::Image image;
	vk::Result result = mDevice.createImage(&imageInfo, nullptr, &image);
	if(result != vk::Result::eSuccess)
    {
		throw std::runtime_error("Failed to create image : "+ vk::to_string(result));
	}
		
	return image;
}


vk::DeviceMemory RendererCore::AllocateImageMemory(
	vk::Image image,
	vk::MemoryPropertyFlags properties ) const
{
	auto memoryRequirements = mDevice.getImageMemoryRequirements(image);
	U32 memoryTypeIndex;
	bool memoryTypeFound = false;
	for(U32 i=0; i<mGpuMemoryProperties.memoryTypeCount; i++) 
    {
		if( memoryRequirements.memoryTypeBits & (1<<i) && 
			(mGpuMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) 
		{
			memoryTypeIndex = i;
			memoryTypeFound = true;
			break;
		}
	}
	if( !memoryTypeFound ) 
    {
		throw std::runtime_error("Failed to find memory type" );
	}
	
	auto allocInfo = vk::MemoryAllocateInfo{}
		.setAllocationSize( memoryRequirements.size )
		.setMemoryTypeIndex( memoryTypeIndex );
		
	vk::DeviceMemory imageMemory;
	vk::Result result = mDevice.allocateMemory(&allocInfo, nullptr, &imageMemory);
	if(result != vk::Result::eSuccess)
    {
		throw std::runtime_error("Failed to allocate image memory : "+ vk::to_string(result));
	}
	
	mDevice.bindImageMemory(image, imageMemory, 0);
	
	return imageMemory;
}


vk::ImageView RendererCore::CreateImageView(
	vk::Image image, vk::Format format,
	vk::ImageAspectFlags aspectFlags,
	U32 mipLevels ) const
{
	auto imageViewInfo = vk::ImageViewCreateInfo{}
		.setImage( image )
		.setViewType( vk::ImageViewType::e2D )
		.setFormat( format )
		.setSubresourceRange( vk::ImageSubresourceRange{aspectFlags, 0, mipLevels, 0, 1} );
			
	vk::ImageView imageView;
	vk::Result result = mDevice.createImageView(&imageViewInfo, nullptr, &imageView);
	if(result != vk::Result::eSuccess)
    {
		throw std::runtime_error("Failed to create image view : "+ vk::to_string(result));
	}
		
	return imageView;
}


vk::CommandPool RendererCore::CreateCommandPool(
    U32 queueFamily,
	vk::CommandPoolCreateFlags flags ) const
{
	auto commandPoolInfo = vk::CommandPoolCreateInfo{}
		.setFlags( flags )
		.setQueueFamilyIndex( queueFamily );
		
	vk::CommandPool commandPool;
	vk::Result result = mDevice.createCommandPool(&commandPoolInfo, nullptr, &commandPool);
	if(result != vk::Result::eSuccess) 
    {
		throw std::runtime_error("Failed to create command pool : "+ vk::to_string(result));
	}
	
	return commandPool;   
}


vk::CommandBuffer RendererCore::AllocateCommandBuffer(
    vk::CommandPool commandPool,
    vk::CommandBufferLevel level ) const
{
    auto allocInfo = vk::CommandBufferAllocateInfo{}
        .setCommandPool( commandPool )
        .setCommandBufferCount( 1 )
        .setLevel( level );

    vk::CommandBuffer commandBuffer;
	vk::Result result = mDevice.allocateCommandBuffers(&allocInfo, &commandBuffer);
	if(result != vk::Result::eSuccess)
    {
		throw std::runtime_error("Failed to allocate command buffer : "+ vk::to_string(result));
	}
	
	return commandBuffer;
}


Array<vk::CommandBuffer> RendererCore::AllocateCommandBuffers(
    vk::CommandPool commandPool,
    U32 count,
    vk::CommandBufferLevel level ) const
{
    auto allocInfo = vk::CommandBufferAllocateInfo{}
        .setCommandPool( commandPool )
        .setCommandBufferCount( count )
        .setLevel( level );

    Array<vk::CommandBuffer> commandBuffers(count);
	vk::Result result = mDevice.allocateCommandBuffers(&allocInfo, commandBuffers.Data());
	if(result != vk::Result::eSuccess)
    {
		throw std::runtime_error("Failed to allocate command buffer : "+ vk::to_string(result));
	}
	
	return commandBuffers;
}

    
} // Atuin
