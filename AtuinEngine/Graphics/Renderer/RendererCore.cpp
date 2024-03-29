
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
	CreateQueues();
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
	if (availableFeatures.sampleRateShading)
	{
		enabledFeatures.setSampleRateShading(VK_TRUE);
	}
	if (availableFeatures.depthClamp)
	{
		enabledFeatures.setDepthClamp(VK_TRUE);
	}
	if (availableFeatures.geometryShader)
	{
		enabledFeatures.setGeometryShader(VK_TRUE);
	}
	if (availableFeatures.tessellationShader)
	{
		enabledFeatures.setTessellationShader(VK_TRUE);
	}
	if (availableFeatures.multiDrawIndirect)
	{
		enabledFeatures.setMultiDrawIndirect(VK_TRUE);
	}
	if (availableFeatures.drawIndirectFirstInstance)
	{
		enabledFeatures.setDrawIndirectFirstInstance(VK_TRUE);
	}
	if (availableFeatures.fillModeNonSolid)
	{
		enabledFeatures.setFillModeNonSolid(VK_TRUE);
	}
	if (availableFeatures.shaderFloat64)
	{
		enabledFeatures.setShaderFloat64(VK_TRUE);
	}
	if( availableFeatures.wideLines)
	{
		enabledFeatures.setWideLines(VK_TRUE);
	}
	

    // create device
    auto deviceInfo = vk::DeviceCreateInfo{}
        .setQueueCreateInfoCount( (U32)queueInfos.size() )
        .setPQueueCreateInfos( queueInfos.data() )
        .setPEnabledFeatures( &enabledFeatures )
        .setEnabledExtensionCount( (U32)requiredExtensions.GetSize() )
        .setPpEnabledExtensionNames( requiredExtensions.Data() );


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
	else
	{
		mComputeQueue = mGraphicsQueue;
	}
    if(mQueueFamilies.transferFamily >= 0)
    {
        mDevice.getQueue(mQueueFamilies.transferFamily, 0, &mTransferQueue);
    }
	else
	{
		mTransferQueue = mGraphicsQueue;
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
    U32 layers,
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
		.setArrayLayers( layers )
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
	U32 baseMipLevel,
	U32 mipLevels,
	U32 baseLayer,
	U32 layerCount,
	vk::ImageViewType viewType ) const
{
	auto imageViewInfo = vk::ImageViewCreateInfo{}
		.setImage( image )
		.setViewType( viewType )
		.setFormat( format )
		.setSubresourceRange( vk::ImageSubresourceRange{aspectFlags, baseMipLevel, mipLevels, baseLayer, layerCount} );
			
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

    Array<vk::CommandBuffer> commandBuffers;
	commandBuffers.Resize(count);
	vk::Result result = mDevice.allocateCommandBuffers(&allocInfo, commandBuffers.Data());
	if(result != vk::Result::eSuccess)
    {
		throw std::runtime_error("Failed to allocate command buffer : "+ vk::to_string(result));
	}
	
	return commandBuffers;
}


vk::Fence RendererCore::CreateFence( vk::FenceCreateFlags signaled ) const {

    auto fenceInfo = vk::FenceCreateInfo{}
		.setFlags( signaled );
	
	vk::Fence fence;
	vk::Result result = mDevice.createFence(&fenceInfo, nullptr, &fence);
	if(result != vk::Result::eSuccess)
    {
		throw std::runtime_error("Failed to create fence : "+ vk::to_string(result));
	}
	
	return fence;
}
	
    
vk::Semaphore RendererCore::CreateSemaphore() const {

	auto semaphoreInfo = vk::SemaphoreCreateInfo{};
	
	vk::Semaphore semaphore;
	vk::Result result = mDevice.createSemaphore(&semaphoreInfo, nullptr, &semaphore);
	if(result != vk::Result::eSuccess)
    {
		throw std::runtime_error("Failed to create semaphore : "+ vk::to_string(result));
	}

	return semaphore;	
}


vk::ShaderModule RendererCore::CreateShaderModule( Size codeSize, const char* code ) const {

    auto shaderInfo = vk::ShaderModuleCreateInfo{}
        .setCodeSize( codeSize )
        .setPCode( reinterpret_cast<const U32*>( code ) );

	vk::ShaderModule shaderModule;
	vk::Result result = mDevice.createShaderModule(&shaderInfo, nullptr, &shaderModule);
	if(result != vk::Result::eSuccess)
    {
		throw std::runtime_error("Failed to create shader module : " + vk::to_string(result) );
	}
	
	return shaderModule;
}


vk::DescriptorPool RendererCore::CreateDescriptorPool( 
	U32 maxSets, 
	U32 count, 
	vk::DescriptorPoolSize* poolSizes,
	vk::DescriptorPoolCreateFlags flags ) const 
{
	auto poolInfo = vk::DescriptorPoolCreateInfo{}
		.setMaxSets( maxSets )
		.setPoolSizeCount( count )
		.setPPoolSizes( poolSizes )
		.setFlags( flags );
		
	vk::DescriptorPool descriptorPool;
	vk::Result result = mDevice.createDescriptorPool(&poolInfo, nullptr, &descriptorPool);
	if( result != vk::Result::eSuccess )
	{
		throw std::runtime_error("Failed to create descriptor pool " + vk::to_string(result));
	}

	return descriptorPool;
}


vk::DescriptorSetLayout RendererCore::CreateDescriptorSetLayout( U32 count, vk::DescriptorSetLayoutBinding* bindings ) const {

	auto layoutInfo = vk::DescriptorSetLayoutCreateInfo{}
		.setBindingCount( count )
		.setPBindings( bindings );

	vk::DescriptorSetLayout descriptorSetLayout;
	vk::Result result = mDevice.createDescriptorSetLayout(&layoutInfo, nullptr, &descriptorSetLayout);
	if( result != vk::Result::eSuccess )
	{
		throw std::runtime_error("Failed to create descriptor set layout " + vk::to_string(result));
	}

	return descriptorSetLayout;
}


vk::DescriptorSet RendererCore::AllocateDescriptorSet( vk::DescriptorPool pool, vk::DescriptorSetLayout layout) const {

	auto allocInfo = vk::DescriptorSetAllocateInfo{}
		.setDescriptorPool( pool )
		.setDescriptorSetCount( 1 )
		.setPSetLayouts( &layout );

	vk::DescriptorSet descriptorSet;
	vk::Result result = mDevice.allocateDescriptorSets(&allocInfo, &descriptorSet);
	if(result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to allocate descriptor sets " + vk::to_string(result));
	}

	return descriptorSet;
}


Array<vk::DescriptorSet> RendererCore::AllocateDescriptorSets( vk::DescriptorPool pool, U32 count, vk::DescriptorSetLayout* layouts ) const {

	auto allocInfo = vk::DescriptorSetAllocateInfo{}
		.setDescriptorPool( pool )
		.setDescriptorSetCount( count )
		.setPSetLayouts( layouts );

	Array<vk::DescriptorSet> descriptorSets;
	descriptorSets.Resize(count);
	vk::Result result = mDevice.allocateDescriptorSets(&allocInfo, descriptorSets.Data());
	if(result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to allocate descriptor sets " + vk::to_string(result));
	}

	return descriptorSets;
}


vk::RenderPass RendererCore::CreateRenderPass( Array<vk::AttachmentDescription> &attachments, I32 depthAttachmentIdx) {

    U32 attachmentCount = (U32)attachments.GetSize();
    Array<vk::AttachmentReference> colorRefs;
    for ( U32 i = 0; i < attachmentCount; i++)
    {
        if ( i == (U32)depthAttachmentIdx)
        {
            continue;
        }
        colorRefs.PushBack( vk::AttachmentReference{ i, vk::ImageLayout::eColorAttachmentOptimal});
    }
	auto depthRef = vk::AttachmentReference{ (U32)depthAttachmentIdx, vk::ImageLayout::eDepthStencilAttachmentOptimal};
		
	auto subpass = vk::SubpassDescription{}
		.setPipelineBindPoint( vk::PipelineBindPoint::eGraphics )
		.setColorAttachmentCount( (U32)colorRefs.GetSize() )
		.setPColorAttachments( colorRefs.Data())
		.setPDepthStencilAttachment( depthAttachmentIdx < 0 ? nullptr : &depthRef )
		.setPResolveAttachments( nullptr );
	
	// dependencies
	auto dependency = vk::SubpassDependency{}
		.setSrcSubpass( VK_SUBPASS_EXTERNAL )
		.setDstSubpass( 0 )
		.setSrcStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput |
						  vk::PipelineStageFlagBits::eEarlyFragmentTests )
		.setSrcAccessMask( vk::AccessFlagBits::eNone )
		.setDstStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput |
						  vk::PipelineStageFlagBits::eEarlyFragmentTests )
		.setDstAccessMask( vk::AccessFlagBits::eColorAttachmentWrite |
						   vk::AccessFlagBits::eDepthStencilAttachmentWrite );

    auto renderPassInfo = vk::RenderPassCreateInfo{}
		.setAttachmentCount( attachmentCount )
		.setPAttachments( attachments.Data() )
		.setSubpassCount( 1 )
		.setPSubpasses( &subpass )
		.setDependencyCount( 1 )
		.setPDependencies( &dependency );

	vk::RenderPass renderPass;
	vk::Result result = mDevice.createRenderPass(&renderPassInfo, nullptr, &renderPass);	
	if(result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create render pass : "+ vk::to_string(result));
	}

	return renderPass;
}


vk::Framebuffer RendererCore::CreateFramebuffer( vk::RenderPass renderpass, Array<vk::ImageView> &attachments, U32 imageWidth, U32 imageHeight, U32 layers) {

	auto framebufferInfo = vk::FramebufferCreateInfo{}
        .setRenderPass( renderpass )
        .setAttachmentCount( (U32)attachments.GetSize() )
        .setPAttachments( attachments.Data() )
        .setWidth( imageWidth )
        .setHeight( imageHeight )
        .setLayers( layers );

	vk::Framebuffer framebuffer;
    vk::Result result = mDevice.createFramebuffer(&framebufferInfo, nullptr, &framebuffer);	
    if(result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to create framebuffer : "+ vk::to_string(result));
    }

	return framebuffer;
}


void RendererCore::PrepareSwapchain( Swapchain &swapchain ) const {

	auto capabilities = mGpu.getSurfaceCapabilitiesKHR( mSurface );
	auto formats      = mGpu.getSurfaceFormatsKHR( mSurface );
	auto presentModes = mGpu.getSurfacePresentModesKHR( mSurface );
	
	// find image count
	swapchain.imageCount = capabilities.minImageCount + 1;
	if(capabilities.maxImageCount > 0)
	{
		swapchain.imageCount = std::min(swapchain.imageCount, capabilities.maxImageCount);
	}
	
	// find image format and color space, default to first one available
	swapchain.imageFormat = formats[0].format;
	swapchain.colorSpace  = formats[0].colorSpace;
	for (const auto format : formats) 
	{
		if ( format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear )
		{
			 swapchain.imageFormat = format.format;
			 swapchain.colorSpace  = format.colorSpace;
			 break;
		}
	}

	// find present mode
#ifdef NDEBUG
	swapchain.presentMode = vk::PresentModeKHR::eFifo;
#else
	swapchain.presentMode = vk::PresentModeKHR::eImmediate;
#endif

}


void RendererCore::CreateSwapchain( Swapchain &swapchain ) const {

	// update swapchain extent to current window size
	swapchain.extent = mGpu.getSurfaceCapabilitiesKHR(mSurface).currentExtent;
	
	auto createInfo = vk::SwapchainCreateInfoKHR{}
		.setSurface( mSurface )
		.setMinImageCount( swapchain.imageCount )
		.setImageFormat( swapchain.imageFormat )
		.setImageColorSpace( swapchain.colorSpace )
		.setPresentMode( swapchain.presentMode )
		.setImageExtent( swapchain.extent )
		.setImageArrayLayers( 1 )										// single image layer
		.setImageUsage( vk::ImageUsageFlagBits::eColorAttachment )		// render target for color
		.setPreTransform( vk::SurfaceTransformFlagBitsKHR::eIdentity )  // no pretransform
		.setCompositeAlpha( vk::CompositeAlphaFlagBitsKHR::eOpaque )	// opaque surface
		.setClipped( VK_TRUE )											// no rendering to non-visible areas
	    .setImageSharingMode( vk::SharingMode::eExclusive )
		.setOldSwapchain( swapchain.swapchain );
	
	
	vk::Result result = mDevice.createSwapchainKHR(&createInfo, nullptr, &swapchain.swapchain);	
	if(result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create swap chain : "+ vk::to_string(result));
	}

	// create swapchain images
	swapchain.images.Resize( swapchain.imageCount);
	swapchain.imageViews.Resize( swapchain.imageCount);
	auto images = mDevice.getSwapchainImagesKHR(swapchain.swapchain);
	for (Size i = 0; i < swapchain.imageCount; i++)
	{
		swapchain.images[i] = images[i];
		swapchain.imageViews[i] = CreateImageView( swapchain.images[i], swapchain.imageFormat, vk::ImageAspectFlagBits::eColor);
	}
}

    
} // Atuin
