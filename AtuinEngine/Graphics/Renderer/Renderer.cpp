
#include "Renderer.h"
#include "RendererCore.h"
#include "Core/Util/Types.h"
#include "Core/Config/ConfigManager.h"

#include "GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"



namespace Atuin {


CVar<U32>* Renderer::pFrameOverlap = ConfigManager::RegisterCVar("Renderer", "FRAME_OVERLAP", (U32)2);


Renderer::Renderer() : 
	mLog(), 
	mMemory(), 
	mJobs(), 
	pWindow {nullptr}, 
	pCore {nullptr},
	mFrameCount {0}
{

}


Renderer::~Renderer() {


}


void Renderer::StartUp(GLFWwindow *window) {

	pWindow = window;
	pCore = mMemory.New<RendererCore>(pWindow);

	pCore->PrepareSwapchain(mSwapchain);
	pCore->CreateSwapchain(mSwapchain);
    
	CreateDepthResources();
	CreateRenderPass();	
	CreateFramebuffers();

	CreateVertexBuffer();
	CreateIndexBuffer();

	CreateFrameResources();
	CreateSubmitContexts();
	CreateShaderModules();
	CreateSamplers();
	CreateDescriptorResources();

	CreateDescriptorSetLayouts();
	CreateDescriptorPool();
	CreateDescriptorSets();

	CreatePipeline();

	// dummy model load
	LoadModel( "../../Resources/Meshes/cube.obj");
	UploadBufferData(mVertices.Data(), mVertices.GetSize() * sizeof(mVertices[0]), mVertexBuffer.buffer);
	UploadBufferData(mIndices.Data(), mIndices.GetSize() * sizeof(mIndices[0]), mIndexBuffer.buffer);
}


void Renderer::ShutDown() {

	// render related
	pCore->Device().destroyPipeline( mSingleMaterialPipeline.pipeline);
	pCore->Device().destroyPipelineLayout( mSingleMaterialPipeline.pipelineLayout);

	pCore->Device().destroyDescriptorSetLayout( mCameraDataLayout);
	pCore->Device().destroyDescriptorSetLayout( mObjectDataLayout);
	pCore->Device().destroyDescriptorSetLayout( mMaterialDataLayout); 

	pCore->Device().destroyDescriptorPool( mDescriptorPool);

	for (auto &framebuffer : mFramebuffers)
	{
		pCore->Device().destroyFramebuffer(framebuffer);
	}	

	pCore->Device().destroyRenderPass( mRenderPass);

	// presentation related
	pCore->Device().destroyImageView( mDepthImage.imageView);
	pCore->Device().destroyImage( mDepthImage.image);
	pCore->Device().freeMemory( mDepthImage.imageMemory);

	for (auto imageView : mSwapchain.imageViews)
	{
		pCore->Device().destroyImageView( imageView);
	}
	pCore->Device().destroySwapchainKHR( mSwapchain.swapchain, nullptr);

	// resources
	pCore->Device().destroyBuffer( mVertexBuffer.buffer);
	pCore->Device().freeMemory( mVertexBuffer.bufferMemory);

	pCore->Device().destroyBuffer( mIndexBuffer.buffer);
	pCore->Device().freeMemory( mIndexBuffer.bufferMemory);

	pCore->Device().destroySampler( mSampler);

	pCore->Device().destroyShaderModule( mMeshVertShader);
	pCore->Device().destroyShaderModule( mMaterialFragShader);

	pCore->Device().destroyBuffer( mCameraBuffer.buffer);
	pCore->Device().freeMemory( mCameraBuffer.bufferMemory);

	pCore->Device().destroyBuffer( mObjectBuffer.buffer);
	pCore->Device().freeMemory( mObjectBuffer.bufferMemory);

	pCore->Device().destroyImageView( mMaterialDiffuseImage.imageView);
	pCore->Device().destroyImage( mMaterialDiffuseImage.image);
	pCore->Device().freeMemory( mMaterialDiffuseImage.imageMemory);

	pCore->Device().destroyImageView( mMaterialNormalImage.imageView);
	pCore->Device().destroyImage( mMaterialNormalImage.image);
	pCore->Device().freeMemory( mMaterialNormalImage.imageMemory);

	pCore->Device().destroyImageView( mMaterialSpecularImage.imageView);
	pCore->Device().destroyImage( mMaterialSpecularImage.image);
	pCore->Device().freeMemory( mMaterialSpecularImage.imageMemory);
	
	for(auto &frame : mFrames) 
	{
		pCore->Device().destroyFence( frame.renderFence, nullptr);
		pCore->Device().destroySemaphore( frame.renderSemaphore, nullptr);
		pCore->Device().destroySemaphore( frame.presentSemaphore, nullptr);
		
		pCore->Device().destroyCommandPool( frame.commandPool);
	}


	pCore->Device().destroyFence( mGraphicsSubmit.fence);
	pCore->Device().destroyCommandPool( mGraphicsSubmit.commandPool);

	pCore->Device().destroyFence( mTransferSubmit.fence);
	pCore->Device().destroyCommandPool( mTransferSubmit.commandPool);

	// core
	mMemory.Delete(pCore);
}
    

void Renderer::Update() {

	++mFrameCount;
}


void Renderer::CreateDepthResources() {

	mDepthImage.format     = vk::Format::eD32Sfloat;
	mDepthImage.usage      = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	mDepthImage.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	
	mDepthImage.image = pCore->CreateImage(
		mSwapchain.extent.width, mSwapchain.extent.height, 
		mDepthImage.format, mDepthImage.usage
	);
	
	mDepthImage.imageMemory = pCore->AllocateImageMemory(
		mDepthImage.image, mDepthImage.memoryType
	);
	
	mDepthImage.imageView = pCore->CreateImageView(
		mDepthImage.image, mDepthImage.format, vk::ImageAspectFlagBits::eDepth
	);
}


void Renderer::CreateRenderPass() {

	// TODO placeholder default render pass -> set up deferred renderer

	// attachements
	auto presentAttachment = vk::AttachmentDescription{}
		.setFormat( mSwapchain.imageFormat )
		.setSamples( vk::SampleCountFlagBits::e1 )
		.setLoadOp( vk::AttachmentLoadOp::eClear )
		.setStoreOp( vk::AttachmentStoreOp::eStore )
		.setStencilLoadOp( vk::AttachmentLoadOp::eDontCare )
		.setStencilStoreOp( vk::AttachmentStoreOp::eDontCare )
		.setInitialLayout( vk::ImageLayout::eUndefined )
		.setFinalLayout( vk::ImageLayout::ePresentSrcKHR );
		
	auto depthAttachment = vk::AttachmentDescription{}
		.setFormat( mDepthImage.format )
		.setSamples( vk::SampleCountFlagBits::e1 )
		.setLoadOp( vk::AttachmentLoadOp::eClear )
		.setStoreOp( vk::AttachmentStoreOp::eDontCare )
		.setStencilLoadOp( vk::AttachmentLoadOp::eDontCare )
		.setStencilStoreOp( vk::AttachmentStoreOp::eDontCare )
		.setInitialLayout( vk::ImageLayout::eUndefined )
		.setFinalLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );
		
	// subpasses
	auto presentAttachmentRef = vk::AttachmentReference{}
		.setAttachment( 0 )
		.setLayout( vk::ImageLayout::eColorAttachmentOptimal );
		
	auto depthAttachmentRef = vk::AttachmentReference{}
		.setAttachment( 1 )
		.setLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );
	
		
	auto subpass = vk::SubpassDescription{}
		.setPipelineBindPoint( vk::PipelineBindPoint::eGraphics )
		.setColorAttachmentCount( 1 )
		.setPColorAttachments( &presentAttachmentRef )
		.setPDepthStencilAttachment( &depthAttachmentRef )
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

	vk::AttachmentDescription attachments[] = {presentAttachment, depthAttachment};
	mRenderPass = pCore->CreateRenderPass(2, attachments, 1, &subpass, 1, &dependency);
}


void Renderer::CreateFramebuffers() {

	mFramebuffers.Resize(mSwapchain.imageCount);
	for(Size i=0; i < mSwapchain.imageCount; i++)
	{
		vk::ImageView attachments[] = {
			mSwapchain.imageViews[i],
			mDepthImage.imageView
		};

		mFramebuffers[i] = pCore->createFrameBuffer(
			mRenderPass, 
			2, attachments, 
			mSwapchain.extent.width, mSwapchain.extent.height
		);
	}
}


void Renderer::RecreateSwapchain() {

	pCore->Device().waitIdle();

	// cleanup old objects	
	// framebuffers
	for(auto &framebuffer : mFramebuffers) {
		pCore->Device().destroyFramebuffer(framebuffer, nullptr);
	}
	
	// depth image
	pCore->Device().destroyImageView(mDepthImage.imageView, nullptr);
	pCore->Device().destroyImage(mDepthImage.image, nullptr);
	pCore->Device().freeMemory(mDepthImage.imageMemory, nullptr);	

	// swapchain
	for(auto imageView : mSwapchain.imageViews) 
	{
	    pCore->Device().destroyImageView( imageView, nullptr);
	}

	vk::SwapchainKHR oldSwapchain = mSwapchain.swapchain;
	pCore->CreateSwapchain(mSwapchain);
	pCore->Device().destroySwapchainKHR( oldSwapchain, nullptr);	
		
	// recreate presentation objects
	CreateDepthResources();
	CreateFramebuffers();
	pCore->CreateSwapchain( mSwapchain);
}


void Renderer::CreateVertexBuffer() {

	mVertexBuffer.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
	mVertexBuffer.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mVertexBuffer.buffer = pCore->CreateBuffer( 8*sizeof(Vertex), mVertexBuffer.usage);
	mVertexBuffer.bufferMemory = pCore->AllocateBufferMemory( mVertexBuffer.buffer, mVertexBuffer.memoryType);
}


void Renderer::CreateIndexBuffer() {

	mIndexBuffer.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
	mIndexBuffer.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mIndexBuffer.buffer = pCore->CreateBuffer( 36*4, mIndexBuffer.usage);
	mIndexBuffer.bufferMemory = pCore->AllocateBufferMemory( mIndexBuffer.buffer, mIndexBuffer.memoryType);
}


void Renderer::LoadModel(std::string_view path) {

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning, error;
		
	if( !tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, path.data()) ) 
	{
		throw std::runtime_error(warning + error);
	}
		
	std::unordered_map<glm::vec3, uint32_t> uniqueVertices{};
		
	for(const auto &shape : shapes) 
	{
		for(const auto &index : shape.mesh.indices) 
		{
			Vertex vertex{};				
			vertex.position = {
				attrib.vertices[3*index.vertex_index + 0],
				attrib.vertices[3*index.vertex_index + 1],
				attrib.vertices[3*index.vertex_index + 2]
			};
			vertex.texCoord = {
				attrib.texcoords[2*index.texcoord_index + 0],
				1.0f - attrib.texcoords[2*index.texcoord_index + 1]
			};
			vertex.normal = {
				attrib.normals[3*index.normal_index + 0],
				attrib.normals[3*index.normal_index + 1],
				attrib.normals[3*index.normal_index + 2]
			};
			vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
				
			if(uniqueVertices.find(vertex.position) == uniqueVertices.end()) 
			{
				uniqueVertices[vertex.position] = static_cast<uint32_t>(mVertices.GetSize());
				mVertices.PushBack(vertex);
			}
				
			mIndices.PushBack(uniqueVertices[vertex.position]);
		}
	}
}


void Renderer::UploadBufferData( void *bufferData, Size size, vk::Buffer targetBuffer, Size offset) {

	Buffer stagingBuffer = CreateStagingBuffer( size);

	// transfer image data to staging buffer
	void *data;
	vk::Result result = pCore->Device().mapMemory( stagingBuffer.bufferMemory, 0, size, vk::MemoryMapFlags(), &data);
	if( result == vk::Result::eSuccess ) 
	{
		memcpy(data, bufferData, size);
		pCore->Device().unmapMemory( stagingBuffer.bufferMemory);
	}
	else 
	{
		throw std::runtime_error("Failed to map staging buffer memory " + vk::to_string(result));
	}

	CopyBuffer( stagingBuffer.buffer, targetBuffer, offset, size);
	
	pCore->Device().destroyBuffer( stagingBuffer.buffer, nullptr);
	pCore->Device().freeMemory( stagingBuffer.bufferMemory, nullptr);
}


Buffer Renderer::CreateStagingBuffer(Size bufferSize) {

	Buffer stagingBuffer;
	stagingBuffer.usage = vk::BufferUsageFlagBits::eTransferSrc;
	stagingBuffer.memoryType = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	stagingBuffer.buffer = pCore->CreateBuffer( bufferSize, stagingBuffer.usage);
	stagingBuffer.bufferMemory = pCore->AllocateBufferMemory( stagingBuffer.buffer, stagingBuffer.memoryType);

	return stagingBuffer;
}


void Renderer::CreateFrameResources() {

	mFrames.Resize( pFrameOverlap->Get());
	for (auto &frame : mFrames)
	{
		frame.renderFence = pCore->CreateFence();
		frame.renderSemaphore = pCore->CreateSemaphore();
		frame.presentSemaphore = pCore->CreateSemaphore();

		frame.commandPool = pCore->CreateCommandPool( pCore->QueueFamilies().graphicsFamily);
		frame.commandBuffer = pCore->AllocateCommandBuffer( frame.commandPool);
	}	
}


void Renderer::CreateSubmitContexts() {

	mGraphicsSubmit.fence = pCore->CreateFence();
	mGraphicsSubmit.commandPool = pCore->CreateCommandPool( pCore->QueueFamilies().graphicsFamily);
	mGraphicsSubmit.commandBuffer = pCore->AllocateCommandBuffer( mGraphicsSubmit.commandPool);

	mTransferSubmit.fence = pCore->CreateFence();
	mTransferSubmit.commandPool = pCore->CreateCommandPool( pCore->QueueFamilies().transferFamily);
	mTransferSubmit.commandBuffer = pCore->AllocateCommandBuffer( mTransferSubmit.commandPool);
}


void Renderer::CreateSamplers() {

	mSampler = pCore->CreateSampler(
		vk::Filter::eLinear, vk::Filter::eLinear,
		vk::SamplerMipmapMode::eNearest,
		vk::SamplerAddressMode::eClampToBorder, vk::SamplerAddressMode::eClampToBorder, vk::SamplerAddressMode::eClampToBorder,
		false
	);
}


void Renderer::CreateDescriptorResources() {

	mCameraBuffer.usage = vk::BufferUsageFlagBits::eUniformBuffer;
	mCameraBuffer.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mCameraBuffer.buffer = pCore->CreateBuffer( sizeof(CameraData), mCameraBuffer.usage);
	mCameraBuffer.bufferMemory = pCore->AllocateBufferMemory(mCameraBuffer.buffer, mCameraBuffer.memoryType);

	CreateImageResource( mMaterialDiffuseImage, "../../Resources/Materials/brick/brick_diffuse.png");
	CreateImageResource( mMaterialNormalImage, "../../Resources/Materials/brick/brick_normal.png");
	CreateImageResource( mMaterialSpecularImage, "../../Resources/Materials/brick/brick_specular.png");

	mObjectBuffer.usage = vk::BufferUsageFlagBits::eUniformBuffer;
	mObjectBuffer.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mObjectBuffer.buffer = pCore->CreateBuffer( sizeof(ObjectData), mObjectBuffer.usage);
	mObjectBuffer.bufferMemory = pCore->AllocateBufferMemory(mObjectBuffer.buffer, mObjectBuffer.memoryType);
}


void Renderer::CreateImageResource(ImageResource &image, std::string_view path) {

	// load image data from file
	int width, height, channels;
	stbi_uc *pixels = stbi_load(path.data(), &width, &height, &channels, STBI_rgb_alpha);
	if (!pixels)
	{
		throw std::runtime_error("Failed to load texture image!");
	}

	// create staging buffer
	Size imageSize = width * height * 4;
	Buffer stagingBuffer = CreateStagingBuffer( imageSize);

	// transfer image data to staging buffer
	void *data;
	vk::Result result = pCore->Device().mapMemory( stagingBuffer.bufferMemory, 0, imageSize, vk::MemoryMapFlags(), &data);
	if( result == vk::Result::eSuccess ) 
	{
		memcpy(data, pixels, imageSize);
		pCore->Device().unmapMemory( stagingBuffer.bufferMemory);
	}
	else 
	{
		throw std::runtime_error("Failed to map image staging buffer memory " + vk::to_string(result));
	}

	// create image on device memory
	vk::Format format;
	switch (channels)
	{
		case 4:
			format = vk::Format::eR8G8B8A8Srgb;
			break;
		case 3:
			format = vk::Format::eR8G8B8Srgb;
			break;
		case 2:
			format = vk::Format::eR16G16Sfloat;
			break;
		case 1:
			format = vk::Format::eR32Sfloat;
			break;
	
		default:
			break;
	}
	image.width = width;
	image.height = height;
	image.format = format;
	image.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
	image.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	image.image = pCore->CreateImage( image.width, image.height, image.format, image.usage);
	image.imageMemory = pCore->AllocateImageMemory( image.image, image.memoryType);
	image.imageView = pCore->CreateImageView(image.image, image.format, vk::ImageAspectFlagBits::eColor);

	// transfer image data from staging buffer
	TransitionImageLayout( image.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	CopyBufferToImage( stagingBuffer.buffer, image.image, image.width, image.height);
	TransitionImageLayout( image.image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

	//cleanup	
	stbi_image_free(pixels);
	pCore->Device().destroyBuffer( stagingBuffer.buffer, nullptr);
	pCore->Device().freeMemory( stagingBuffer.bufferMemory, nullptr);
}


void Renderer::CreateDescriptorSetLayouts() {

	auto cameraDataBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 0 )
		.setDescriptorType( vk::DescriptorType::eUniformBuffer )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eVertex );

	mCameraDataLayout = pCore->CreateDescriptorSetLayout(1, &cameraDataBinding);
	
			
	auto diffuseSamplerBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 0 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eFragment );
	auto normalSamplerBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 1 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eFragment );		
	auto specularSamplerBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 2 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eFragment );

	vk::DescriptorSetLayoutBinding materialBindings[] = {
		diffuseSamplerBinding, normalSamplerBinding, specularSamplerBinding
	};
	mMaterialDataLayout = pCore->CreateDescriptorSetLayout(3, materialBindings);


	auto objectDataBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 0 )
		.setDescriptorType( vk::DescriptorType::eUniformBuffer )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eVertex );
	
	mObjectDataLayout = pCore->CreateDescriptorSetLayout(1, &objectDataBinding);
}


void Renderer::CreateDescriptorPool() {

	vk::DescriptorPoolSize poolSizes[] = {

		{vk::DescriptorType::eUniformBuffer,  10 * pFrameOverlap->Get()},
		{vk::DescriptorType::eCombinedImageSampler,  10 * pFrameOverlap->Get()}
	};

	mDescriptorPool = pCore->CreateDescriptorPool(10*pFrameOverlap->Get(), 2, poolSizes);
}


void Renderer::CreateDescriptorSets() {

	vk::DescriptorSetLayout descriptorSetLayouts[] = {
		mCameraDataLayout, mMaterialDataLayout, mObjectDataLayout
	};
	auto descriptorSets = pCore->AllocateDescriptorSets(mDescriptorPool, 3, descriptorSetLayouts);

	mCameraDataSet = descriptorSets[0];
	mMaterialDataSet = descriptorSets[1];
	mObjectDataSet = descriptorSets[2];


	auto cameraInfo = vk::DescriptorBufferInfo{}
		.setBuffer( mCameraBuffer.buffer )
		.setOffset(0)
		.setRange( sizeof(CameraData) );
	auto cameraWrite = vk::WriteDescriptorSet{}
		.setDstSet( mCameraDataSet )
		.setDstBinding( 0 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eUniformBuffer )
		.setPBufferInfo( &cameraInfo );

	auto materialDiffuseInfo = vk::DescriptorImageInfo{}
		.setImageLayout( vk::ImageLayout::eShaderReadOnlyOptimal )
		.setImageView( mMaterialDiffuseImage.imageView )
		.setSampler( mSampler );
	auto materialDiffuseWrite = vk::WriteDescriptorSet{}
		.setDstSet( mMaterialDataSet )
		.setDstBinding( 0 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setPImageInfo( &materialDiffuseInfo );
	auto materialNormalInfo = vk::DescriptorImageInfo{}
		.setImageLayout( vk::ImageLayout::eShaderReadOnlyOptimal )
		.setImageView( mMaterialNormalImage.imageView )
		.setSampler( mSampler );
	auto materialNormalWrite = vk::WriteDescriptorSet{}
		.setDstSet( mMaterialDataSet )
		.setDstBinding( 1 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setPImageInfo( &materialNormalInfo );
	auto materialSpecularInfo = vk::DescriptorImageInfo{}
		.setImageLayout( vk::ImageLayout::eShaderReadOnlyOptimal )
		.setImageView( mMaterialSpecularImage.imageView )
		.setSampler( mSampler );
	auto materialSpecularWrite = vk::WriteDescriptorSet{}
		.setDstSet( mMaterialDataSet )
		.setDstBinding( 2 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setPImageInfo( &materialSpecularInfo );

	auto objectInfo = vk::DescriptorBufferInfo{}
		.setBuffer( mObjectBuffer.buffer )
		.setOffset(0)
		.setRange( sizeof(ObjectData) );
	auto objectWrite = vk::WriteDescriptorSet{}
		.setDstSet( mObjectDataSet )
		.setDstBinding( 0 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eUniformBuffer )
		.setPBufferInfo( &objectInfo );	

	vk::WriteDescriptorSet descriptorWrites[] = {
		cameraWrite, materialDiffuseWrite, materialNormalWrite, materialSpecularWrite, objectWrite
	};
	pCore->Device().updateDescriptorSets(5, descriptorWrites, 0, nullptr);
}


void Renderer::CreateShaderModules() {

	auto vertShaderCode = mFiles.Read( "../../Resources/Shaders/single_mesh_vert.spv" , std::ios::binary);
	auto fragShaderCode = mFiles.Read( "../../Resources/Shaders/single_material_frag.spv" , std::ios::binary);

	mMeshVertShader = pCore->CreateShaderModule( vertShaderCode.GetSize(), vertShaderCode.Data() );
	mMaterialFragShader = pCore->CreateShaderModule( fragShaderCode.GetSize(), fragShaderCode.Data() );
}


void Renderer::CreatePipeline() {

	// shader stages
	mSingleMaterialPipeline.shaderInfos = {
		vk::PipelineShaderStageCreateInfo{}
			.setStage( vk::ShaderStageFlagBits::eVertex )
			.setModule( mMeshVertShader )
			.setPName( "main" ),
			
		vk::PipelineShaderStageCreateInfo{}
			.setStage( vk::ShaderStageFlagBits::eFragment )
			.setModule( mMaterialFragShader )
			.setPName( "main" )
	};
	
	// vertex input
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();
	mSingleMaterialPipeline.vertexInputInfo
		.setVertexBindingDescriptionCount( 1 )
		.setPVertexBindingDescriptions( &bindingDescription )
		.setVertexAttributeDescriptionCount( (U32)attributeDescriptions.GetSize() )
		.setPVertexAttributeDescriptions( attributeDescriptions.Data() );			
			
	// input assembly
	mSingleMaterialPipeline.inputAssemblyInfo
		.setTopology( vk::PrimitiveTopology::eTriangleList )
		.setPrimitiveRestartEnable( VK_FALSE );

	mSingleMaterialPipeline.viewportInfo
		.setViewportCount( 1 )
		.setScissorCount( 1 );
			
	// rasterization
	mSingleMaterialPipeline.rasterizerInfo
		.setDepthClampEnable( VK_FALSE )
		.setRasterizerDiscardEnable( VK_FALSE )
		.setPolygonMode( vk::PolygonMode::eFill )
		.setLineWidth( 1.0 )
		.setCullMode( vk::CullModeFlagBits::eNone )
		.setFrontFace( vk::FrontFace::eClockwise )
		.setDepthBiasEnable( VK_FALSE )
		.setDepthBiasConstantFactor( 0.0f )
		.setDepthBiasClamp( 0.0f )
		.setDepthBiasSlopeFactor( 0.0f );
		
	// multisampling
	mSingleMaterialPipeline.multisampleInfo
		.setSampleShadingEnable( VK_FALSE )
		.setMinSampleShading( 1.0f )
		.setRasterizationSamples( vk::SampleCountFlagBits::e1)
		.setPSampleMask( nullptr )
		.setAlphaToCoverageEnable( VK_FALSE )
		.setAlphaToOneEnable( VK_FALSE );		
		
	// depth and stencil
	mSingleMaterialPipeline.depthStencilInfo
		.setDepthTestEnable( VK_TRUE )
		.setDepthWriteEnable( VK_TRUE )
		.setDepthCompareOp( vk::CompareOp::eLessOrEqual )
		.setDepthBoundsTestEnable( VK_FALSE ) 
		.setMinDepthBounds( 0.0f )
		.setMaxDepthBounds( 1.0f )
		.setStencilTestEnable( VK_FALSE )
		.setFront( {} )
		.setBack( {} );	
				
	// color blending
	mSingleMaterialPipeline.colorBlendAttachment
		.setColorWriteMask( vk::ColorComponentFlagBits::eR |
							vk::ColorComponentFlagBits::eG |
							vk::ColorComponentFlagBits::eB |
							vk::ColorComponentFlagBits::eA )
		.setBlendEnable( VK_TRUE )
		.setSrcColorBlendFactor( vk::BlendFactor::eSrcAlpha )
		.setDstColorBlendFactor( vk::BlendFactor::eOneMinusSrcAlpha )
		.setColorBlendOp( vk::BlendOp::eAdd )
		.setSrcAlphaBlendFactor( vk::BlendFactor::eOne )
		.setDstAlphaBlendFactor( vk::BlendFactor::eZero )
		.setAlphaBlendOp( vk::BlendOp::eAdd );
		
	mSingleMaterialPipeline.colorBlendInfo
		.setAttachmentCount( 1 )
		.setPAttachments( &mSingleMaterialPipeline.colorBlendAttachment );

	// dynamic states
	vk::DynamicState dynamicState[] = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
	mSingleMaterialPipeline.dynamicStateInfo
		.setDynamicStateCount( 2 )
		.setPDynamicStates( dynamicState );

	// pipeline layout
	vk::DescriptorSetLayout descriptorSetLayouts[] = {
		mCameraDataLayout, mMaterialDataLayout, mObjectDataLayout
	};
	mSingleMaterialPipeline.pipelineLayout = pCore->CreatePipelineLayout( 3, descriptorSetLayouts );

	// render pass
	mSingleMaterialPipeline.renderpass = mRenderPass;
	mSingleMaterialPipeline.subpass = 0;

	pCore->CreatePipeline( mSingleMaterialPipeline );
}


void Renderer::TransitionImageLayout(vk::Image image, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout, U32 mipLevels) {

	vk::CommandBuffer cmd = mGraphicsSubmit.commandBuffer;
	auto beginInfo = vk::CommandBufferBeginInfo{}
		.setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );
	
	cmd.begin( beginInfo );
	
	auto barrier = vk::ImageMemoryBarrier{}
		.setOldLayout( initialLayout ) 
		.setNewLayout( finalLayout )
		.setImage( image )
		.setSubresourceRange( vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, mipLevels, 0, 1} )
		.setSrcQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED )
		.setDstQueueFamilyIndex( VK_QUEUE_FAMILY_IGNORED );

		
	vk::PipelineStageFlags sourceStage, destinationStage;
	if (initialLayout == vk::ImageLayout::eUndefined && finalLayout == vk::ImageLayout::eTransferDstOptimal ) 
	{	
		barrier.setSrcAccessMask( vk::AccessFlagBits::eNone );
		barrier.setDstAccessMask( vk::AccessFlagBits::eTransferWrite );
		
		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe; 
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if( initialLayout == vk::ImageLayout::eTransferDstOptimal && finalLayout == vk::ImageLayout::eShaderReadOnlyOptimal ) 
	{
		barrier.setSrcAccessMask( vk::AccessFlagBits::eTransferWrite );
		barrier.setDstAccessMask( vk::AccessFlagBits::eShaderRead );
		
		sourceStage = vk::PipelineStageFlagBits::eTransfer; 
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else 
	{
		throw std::runtime_error("unsupported layout transition!");
	}
		
	cmd.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags{}, 0, nullptr, 0, nullptr, 1, &barrier);

	cmd.end();

	auto submitInfo = vk::SubmitInfo{}
		.setCommandBufferCount( 1 )
		.setPCommandBuffers( &cmd );
	
	vk::Result result;
	result = pCore->GraphicsQueue().submit( 1, &submitInfo, mGraphicsSubmit.fence);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while rsubmitting to graphics queue " + vk::to_string(result));
	}

	result = pCore->Device().waitForFences( 1, &mGraphicsSubmit.fence, true, 1e9);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while waiting for fence " + vk::to_string(result));
	}
	result = pCore->Device().resetFences( 1, &mGraphicsSubmit.fence);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while resetting fence " + vk::to_string(result));
	}
	pCore->Device().resetCommandPool( mGraphicsSubmit.commandPool);
}


void Renderer::CopyBufferToImage(vk::Buffer buffer, vk::Image image, U32 imageWidth, U32 imageHeight) {

	vk::CommandBuffer cmd = mTransferSubmit.commandBuffer;
	auto beginInfo = vk::CommandBufferBeginInfo{}
		.setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );
	
	cmd.begin( beginInfo);
	
	auto copyRegion = vk::BufferImageCopy{}
		.setBufferOffset( 0 )
		.setBufferRowLength( 0 )
		.setBufferImageHeight( 0 )
		.setImageSubresource( vk::ImageSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1} )
		.setImageOffset( {0, 0, 0} )
		.setImageExtent( vk::Extent3D{imageWidth, imageHeight, 1} );
		
	cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &copyRegion);
	
	cmd.end();
	
	auto submitInfo = vk::SubmitInfo{}
		.setCommandBufferCount( 1 )
		.setPCommandBuffers( &cmd );
	
	vk::Result result;
	result = pCore->TransferQueue().submit( 1, &submitInfo, mTransferSubmit.fence);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while submitting to transfer queue " + vk::to_string(result));
	}

	result = pCore->Device().waitForFences( 1, &mTransferSubmit.fence, true, 1e9);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while waiting for fence " + vk::to_string(result));
	}
	result = pCore->Device().resetFences( 1, &mTransferSubmit.fence);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while resetting fence " + vk::to_string(result));
	}
	pCore->Device().resetCommandPool( mTransferSubmit.commandPool);
}


void Renderer::CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, Size offset, Size bufferSize) {

	vk::CommandBuffer cmd = mTransferSubmit.commandBuffer;
	auto beginInfo = vk::CommandBufferBeginInfo{}
		.setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );
	
	cmd.begin( beginInfo);
	
	auto copyRegion = vk::BufferCopy{}
		.setSrcOffset( 0 )
		.setDstOffset( offset )
		.setSize( bufferSize );
		
	cmd.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);
	
	cmd.end();
	
	auto submitInfo = vk::SubmitInfo{}
		.setCommandBufferCount( 1 )
		.setPCommandBuffers( &cmd );
	
	vk::Result result;
	result = pCore->TransferQueue().submit( 1, &submitInfo, mTransferSubmit.fence);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while submitting to transfer queue " + vk::to_string(result));
	}

	result = pCore->Device().waitForFences( 1, &mTransferSubmit.fence, true, 1e9);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while waiting for fence " + vk::to_string(result));
	}
	result = pCore->Device().resetFences( 1, &mTransferSubmit.fence);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while resetting fence " + vk::to_string(result));
	}
	pCore->Device().resetCommandPool( mTransferSubmit.commandPool);
}


void Renderer::DrawFrame() {

	auto frame = CurrentFrame();
	UpdateCameraData();
	UpdateObjectData();


	vk::Result result;

	//
	U32 imageIndex;
	result = pCore->Device().acquireNextImageKHR(
		mSwapchain.swapchain, U64_MAX, frame.renderSemaphore, nullptr, &imageIndex
	);
	if( result != vk::Result::eErrorOutOfDateKHR)
	{
		RecreateSwapchain();   // TODO maybe handle in glfwResizeCallback instead ?
		return;
	}
	else if( result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Error while waiting for fence " + vk::to_string(result));
	}
	
	// wait for render fence to record render commands
	result = pCore->Device().waitForFences(frame.renderFence, true, 1e9);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while waiting for render fence " + vk::to_string(result));
	}
	result = pCore->Device().resetFences( 1, &frame.renderFence);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while resetting render fence " + vk::to_string(result));
	}

	// get current frames command buffer and reset
	auto &cmd = frame.commandBuffer;
	cmd.reset( vk::CommandBufferResetFlagBits::eReleaseResources);	


	// record render commands
	auto cmdBeginInfo = vk::CommandBufferBeginInfo{}
		.setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );		
	cmd.begin(cmdBeginInfo);	

	//start renderpass
	vk::ClearValue clearValues[2];
	clearValues[0].setColor( vk::ClearColorValue().setFloat32({0.0f, 0.0f, 0.0f, 1.0f}) );
	clearValues[1].setDepthStencil( {1.0f, 0} ); 

	auto renderInfo = vk::RenderPassBeginInfo{}
		.setRenderPass( mRenderPass )
		.setRenderArea( {{0, 0}, mSwapchain.extent} )
		.setFramebuffer( mFramebuffers[imageIndex] )
		.setClearValueCount( 2 )
		.setPClearValues( clearValues );		
	cmd.beginRenderPass(renderInfo, vk::SubpassContents::eInline);
	
	cmd.bindPipeline(
		vk::PipelineBindPoint::eGraphics, mSingleMaterialPipeline.pipeline
	);

	vk::Buffer vertexBuffers[] = {mVertexBuffer.buffer};
	Size offsets[] = {0};
	cmd.bindVertexBuffers( 0, 1, vertexBuffers, offsets);
	cmd.bindIndexBuffer( mIndexBuffer.buffer, 0, vk::IndexType::eUint32);
		
	vk::DescriptorSet descriptorSets[] = {
		mCameraDataSet, mMaterialDataSet, mObjectDataSet
	};
	cmd.bindDescriptorSets( 
		vk::PipelineBindPoint::eGraphics, mSingleMaterialPipeline.pipelineLayout, 0, 3, descriptorSets, 0, nullptr		
	);
	
	cmd.drawIndexed( (U32)mIndices.GetSize(), 1, 0, 0, 0);
	
	cmd.endRenderPass();
	
	cmd.end();


	// submit render commands
	vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	auto submitInfo = vk::SubmitInfo{}
		.setWaitSemaphoreCount( 1 )
		.setPWaitSemaphores( &frame.renderSemaphore )
		.setPWaitDstStageMask( &waitStage )
		.setCommandBufferCount( 1 )
		.setPCommandBuffers( &cmd )
		.setSignalSemaphoreCount( 1 )
		.setPSignalSemaphores( &frame.presentSemaphore );
		
	result = pCore->GraphicsQueue().submit( 1, &submitInfo, frame.renderFence);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while submitting to graphics queue " + vk::to_string(result));
	}

	// present rendered image 
	auto presentInfo = vk::PresentInfoKHR{}
		.setWaitSemaphoreCount( 1 )
		.setPWaitSemaphores( &frame.presentSemaphore )
		.setSwapchainCount( 1 )
		.setPSwapchains( &mSwapchain.swapchain )
		.setPImageIndices( &imageIndex );
	
	result = pCore->GraphicsQueue().presentKHR( presentInfo);
	if( result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || 
		mSwapchain.extent != pCore->surfaceExtent() ) 
	{
		RecreateSwapchain();
		return;
	}
	else if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while presenting image " + vk::to_string(result));
	}

	++mFrameCount;
}


void Renderer::UpdateCameraData() {

	CameraData camera;
	camera.proj = glm::perspective(
			glm::radians(45.f),
			(float)mSwapchain.extent.width / (float)mSwapchain.extent.height,
			0.1f, 256.0f
		);
	camera.view = glm::lookAt( 
		glm::vec3(2.f, 2.f, 2.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 0.f, 1.f)
	);
	camera.viewProj = camera.proj * camera.view;

	UploadBufferData(&camera, sizeof(CameraData), mCameraBuffer.buffer);
}


void Renderer::UpdateObjectData() {

	glm::mat4 rotation = glm::rotate(
		glm::mat4(1.f), 
		(float)(mFrameCount % 60) / 60.f * glm::radians(30.f), 
		glm::vec3(0.f, 0.f, 1.f)
	);
	glm::mat4 translate = glm::translate(
		glm::mat4(1.f),
		glm::vec3(0.f, 0.f, 0.f)
	);
	glm::mat4 scale = glm::scale(
		glm::mat4(1.f),
		glm::vec3(1.f,1.f,1.f)
	);
	ObjectData obj;
	obj.model = translate * rotation * scale;
	
	UploadBufferData(&obj, sizeof(ObjectData), mObjectBuffer.buffer);
}

    
} // Atuin
