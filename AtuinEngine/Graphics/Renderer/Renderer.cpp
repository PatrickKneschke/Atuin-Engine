
#include "Renderer.h"
#include "RendererCore.h"
#include "ResourceManager.h"
#include "JsonToVulkan.h"
#include "App.h"
#include "Core/Util/Types.h"
#include "Core/Config/ConfigManager.h"
#include "Core/DataStructures/Json.h"

#include "GLFW/glfw3.h"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/string_cast.hpp>


namespace Atuin {


CVar<U32>*   Renderer::pFrameOverlap = ConfigManager::RegisterCVar("Renderer", "FRAME_OVERLAP", (U32)2);
CVar<U32>*   Renderer::pMaxAnisotropy = ConfigManager::RegisterCVar("Renderer", "MAX_ANISOTROPY", (U32)8);
CVar<U32>*   Renderer::pMsaaSamples = ConfigManager::RegisterCVar("Renderer", "MSAA_SAMPLES", (U32)1);
CVar<U32>*   Renderer::pShadowCascadeCount = ConfigManager::RegisterCVar("Renderer", "SHADOW_CASCADE_COUNT", (U32)4);
CVar<float>* Renderer::pShadowCascadeSplit = ConfigManager::RegisterCVar("Renderer", "SHADOW_CASCADE_SPLIT", 0.92f);


Renderer::Renderer() : 
	mLog(), 
	mMemory(), 
	mJobs(), 
	mResourceDir {App::sResourceDir->Get()},
	pWindow {nullptr}, 
	pCore {nullptr},
	pResources {nullptr},
	pDescriptorSetAllocator {nullptr}, 
	pDescriptorLayoutCache {nullptr}, 
	mFrameCount {0},
	mMeshesDirty {false}
{

}


Renderer::~Renderer() {

}


void Renderer::StartUp(GLFWwindow *window) {

	pWindow = window;
	pCore = mMemory.New<RendererCore>(pWindow);
	pResources = mMemory.New<ResourceManager>();
	pDescriptorSetAllocator = mMemory.New<DescriptorSetAllocator>();
	pDescriptorSetAllocator->Init( pCore->Device());
	pDescriptorLayoutCache = mMemory.New<DescriptorLayoutCache>();
	pDescriptorLayoutCache->Init( pCore->Device());

	pCore->PrepareSwapchain(mSwapchain);
	pCore->CreateSwapchain(mSwapchain);

	CreateSubmitContexts();
	CreateFrameResources();
    
	CreateRenderPasses();
	CreateDepthResources();
	CreateShadowResources();
	CreateFramebuffers();

    CreateDefaultPipelineBuilder();
	CreateSamplers();
    CreatePipelines();

	CreateMeshPass( &mShadowMeshPass, PassType::SHADOW);
	CreateMeshPass( &mOpaqueMeshPass, PassType::OPAQUE);
	CreateMeshPass( &mTransparentMeshPass, PassType::TRANSPARENT);

	CreateDescriptorResources();
	CreateDescriptorSetLayouts();
	CreateDescriptorSets();


	// camera
	float unit = 8;
	mMainCamera.position = {10.0f * unit, 10.0f * unit, 10.0f * unit};
	mMainCamera.forward = glm::normalize( glm::vec3(0.f, 0.f, 0.f) - mMainCamera.position);
	mMainCamera.fov = glm::radians( 60.f);
	mMainCamera.aspect = (float)mSwapchain.extent.width / (float)mSwapchain.extent.height;
	mMainCamera.zNear = 0.1f;
	mMainCamera.zFar = 200.f;
	mMainCamera.UpdateCoordinates();
}


void Renderer::ShutDown() {

	pCore->Device().waitIdle();


	// debug
	pCore->Device().destroyBuffer( cullDebugBuffer.buffer);
	pCore->Device().freeMemory(    cullDebugBuffer.bufferMemory);



	mDeletionStack.Flush();
	for ( auto &frame : mFrames) {

		frame.deletionStack.Flush();
	}

	// deletion of sporadically recreated buffers
	pCore->Device().destroyBuffer( mCombinedVertexBuffer.buffer);
	pCore->Device().freeMemory(    mCombinedVertexBuffer.bufferMemory);
	pCore->Device().destroyBuffer( mCombinedIndexBuffer.buffer);
	pCore->Device().freeMemory(    mCombinedIndexBuffer.bufferMemory);
	pCore->Device().destroyBuffer( mObjectBuffer.buffer);
	pCore->Device().freeMemory(    mObjectBuffer.bufferMemory);

	pCore->Device().destroyBuffer( mShadowMeshPass.drawIndirectBuffer.buffer);
	pCore->Device().freeMemory(    mShadowMeshPass.drawIndirectBuffer.bufferMemory);
	pCore->Device().destroyBuffer( mShadowMeshPass.instanceDataBuffer.buffer);
	pCore->Device().freeMemory(    mShadowMeshPass.instanceDataBuffer.bufferMemory);
	pCore->Device().destroyBuffer( mShadowMeshPass.instanceIndexBuffer.buffer);
	pCore->Device().freeMemory(    mShadowMeshPass.instanceIndexBuffer.bufferMemory);

	pCore->Device().destroyBuffer( mOpaqueMeshPass.drawIndirectBuffer.buffer);
	pCore->Device().freeMemory(    mOpaqueMeshPass.drawIndirectBuffer.bufferMemory);
	pCore->Device().destroyBuffer( mOpaqueMeshPass.instanceDataBuffer.buffer);
	pCore->Device().freeMemory(    mOpaqueMeshPass.instanceDataBuffer.bufferMemory);
	pCore->Device().destroyBuffer( mOpaqueMeshPass.instanceIndexBuffer.buffer);
	pCore->Device().freeMemory(    mOpaqueMeshPass.instanceIndexBuffer.bufferMemory);

	pCore->Device().destroyBuffer( mTransparentMeshPass.drawIndirectBuffer.buffer);
	pCore->Device().freeMemory(    mTransparentMeshPass.drawIndirectBuffer.bufferMemory);
	pCore->Device().destroyBuffer( mTransparentMeshPass.instanceDataBuffer.buffer);
	pCore->Device().freeMemory(    mTransparentMeshPass.instanceDataBuffer.bufferMemory);
	pCore->Device().destroyBuffer( mTransparentMeshPass.instanceIndexBuffer.buffer);
	pCore->Device().freeMemory(    mTransparentMeshPass.instanceIndexBuffer.bufferMemory);


	// presentation related
	DestroyFramebuffers();	

	pCore->Device().destroyImageView( mShadowImage.imageView);
	pCore->Device().destroyImage( mShadowImage.image);
	pCore->Device().freeMemory( mShadowImage.imageMemory);

	pCore->Device().destroyImageView( mDepthImage.imageView);
	pCore->Device().destroyImage( mDepthImage.image);
	pCore->Device().freeMemory( mDepthImage.imageMemory);

	for (auto imageView : mDepthPyramidViews)
	{
		pCore->Device().destroyImageView( imageView);
	}
	pCore->Device().destroyImageView( mDepthPyramid.imageView);
	pCore->Device().destroyImage( mDepthPyramid.image);
	pCore->Device().freeMemory( mDepthPyramid.imageMemory);

	for (auto imageView : mSwapchain.imageViews)
	{
		pCore->Device().destroyImageView( imageView);
	}
	pCore->Device().destroySwapchainKHR( mSwapchain.swapchain, nullptr);

	// descriptor 
	pDescriptorLayoutCache->DestroyLayouts();
	mMemory.Delete(( pDescriptorLayoutCache));
	pDescriptorSetAllocator->DestroyPools();
	mMemory.Delete( pDescriptorSetAllocator);

	// core
	mMemory.Delete(pCore);
}
    

void Renderer::Update() {

	// TODO make async calls

	if ( mMeshesDirty)
	{
		MergeMeshes();
		mMeshesDirty = false;
	}

	auto meshUpdates = mJobs.CreateJob( [](void*){}, nullptr);
	auto shadowUpdate = mJobs.CreateJob( [&](void *data){
		UpdateMeshPass( (MeshPass*)data);
	}, &mShadowMeshPass, meshUpdates);
	auto opaqueUpdate = mJobs.CreateJob( [&](void *data){
		UpdateMeshPass( (MeshPass*)data);
	}, &mOpaqueMeshPass, meshUpdates);
	auto transparentUpdate = mJobs.CreateJob( [&](void *data){
		UpdateMeshPass( (MeshPass*)data);
	}, &mTransparentMeshPass, meshUpdates);

	mJobs.Run( shadowUpdate);
	mJobs.Run( opaqueUpdate);
	mJobs.Run( transparentUpdate);
	mJobs.Run( meshUpdates);

	mJobs.Wait( meshUpdates);

	// UpdateMeshPass( &mShadowMeshPass);
	// UpdateMeshPass( &mOpaqueMeshPass);
	// UpdateMeshPass( &mTransparentMeshPass);

	DrawFrame();
}


void Renderer::CreateDepthResources() {

	// depth image
	mDepthImage.format     = vk::Format::eD32Sfloat;
	mDepthImage.usage      = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
	mDepthImage.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mDepthImage.width      = mSwapchain.extent.width;
	mDepthImage.height     = mSwapchain.extent.height; 

	mDepthImage.image = pCore->CreateImage(
		mDepthImage.width, mDepthImage.height, 
		mDepthImage.format, mDepthImage.usage
	);	
	mDepthImage.imageMemory = pCore->AllocateImageMemory(
		mDepthImage.image, mDepthImage.memoryType
	);	
	mDepthImage.imageView = pCore->CreateImageView(
		mDepthImage.image, mDepthImage.format, vk::ImageAspectFlagBits::eDepth
	);

	// depth pyramid

	mDepthPyramid.format = vk::Format::eR32Sfloat;
	mDepthPyramid.usage  = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;
	mDepthPyramid.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mDepthPyramid.width = (U32)Math::PrevPowerOfTwo( (U64)mDepthImage.width);
	mDepthPyramid.height = (U32)Math::PrevPowerOfTwo( (U64)mDepthImage.height);
	
	U32 pyramidLevels = static_cast<uint32_t>(std::floor( std::log2( std::max( mDepthPyramid.width, mDepthPyramid.height)))) + 1;

	mDepthPyramid.image = pCore->CreateImage(
		mDepthPyramid.width, mDepthPyramid.height, 
		mDepthPyramid.format, mDepthPyramid.usage, pyramidLevels
	);	
	mDepthPyramid.imageMemory = pCore->AllocateImageMemory(
		mDepthPyramid.image, mDepthPyramid.memoryType
	);	
	mDepthPyramid.imageView = pCore->CreateImageView(
		mDepthPyramid.image, mDepthPyramid.format, vk::ImageAspectFlagBits::eColor, 0, pyramidLevels
	);

	TransitionImageLayout( mDepthPyramid.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, pyramidLevels);

	mDepthPyramidViews.Resize( pyramidLevels);
	for ( U32 i = 0; i < pyramidLevels; i++)
	{
		mDepthPyramidViews[i] = pCore->CreateImageView(
			mDepthPyramid.image, mDepthPyramid.format, vk::ImageAspectFlagBits::eColor, i, 1
		);
	}
}


void Renderer::CreateShadowResources() {

	mShadowExtent.setWidth( 128).setHeight( 128).setDepth( 1000);

	mShadowImage.format     = mDepthFormat;
	mShadowImage.usage      = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
	mShadowImage.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mShadowImage.width      = 2048;
	mShadowImage.height     = 2048; 

	mShadowImage.image = pCore->CreateImage(
		mShadowImage.width, mShadowImage.height, 
		mShadowImage.format, mShadowImage.usage
	);	
	mShadowImage.imageMemory = pCore->AllocateImageMemory(
		mShadowImage.image, mShadowImage.memoryType
	);	
	mShadowImage.imageView = pCore->CreateImageView(
		mShadowImage.image, mShadowImage.format, vk::ImageAspectFlagBits::eDepth
	);


	// main light 
	// shadow image
	mMainLight.shadowImage.format     = mDepthFormat;
	mMainLight.shadowImage.usage      = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
	mMainLight.shadowImage.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mMainLight.shadowImage.width      = 2048;
	mMainLight.shadowImage.height     = 2048; 

	mMainLight.shadowImage.image = pCore->CreateImage(
		mMainLight.shadowImage.width, mMainLight.shadowImage.height,
		mMainLight.shadowImage.format, mMainLight.shadowImage.usage,
		1, pShadowCascadeCount->Get()
	);	
	mMainLight.shadowImage.imageMemory = pCore->AllocateImageMemory(
		mMainLight.shadowImage.image, mMainLight.shadowImage.memoryType
	);	
	mMainLight.shadowImage.imageView = pCore->CreateImageView(
		mMainLight.shadowImage.image, mMainLight.shadowImage.format, vk::ImageAspectFlagBits::eDepth,
		0, 1, 0, pShadowCascadeCount->Get(),
		vk::ImageViewType::e2DArray
	);
	mDeletionStack.Push( [&](){
		pCore->Device().destroyImage( mMainLight.shadowImage.image);
		pCore->Device().destroyImageView( mMainLight.shadowImage.imageView);
		pCore->Device().freeMemory( mMainLight.shadowImage.imageMemory);
	});

	// cascade view matrix buffer
	CreateBuffer(
		mMainLight.cascadeViewBuffer, pShadowCascadeCount->Get() * sizeof( glm::mat4),
		vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);
	mDeletionStack.Push( [&](){
		pCore->Device().destroyBuffer( mMainLight.cascadeViewBuffer.buffer);
		pCore->Device().freeMemory( mMainLight.cascadeViewBuffer.bufferMemory);
	});

	// cascade viewProj matrix buffer
	CreateBuffer(
		mMainLight.cascadeViewProjBuffer, pShadowCascadeCount->Get() * sizeof( glm::mat4),
		vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);
	mDeletionStack.Push( [&](){
		pCore->Device().destroyBuffer( mMainLight.cascadeViewProjBuffer.buffer);
		pCore->Device().freeMemory( mMainLight.cascadeViewProjBuffer.bufferMemory);
	});

	// cascade image views and framebuffers
	mMainLight.cascades.Resize( pShadowCascadeCount->Get());
	for( U32 i = 0; i < pShadowCascadeCount->Get(); i++)
	{
		// image view for cascade layer in shadow image
		mMainLight.cascades[i].imageView = pCore->CreateImageView(
			mMainLight.shadowImage.image, mMainLight.shadowImage.format, vk::ImageAspectFlagBits::eDepth,
			0, 1, i, 1,
			vk::ImageViewType::e2DArray
		);
		// framebuffer used to render this layer in shadow image
		Array<vk::ImageView> shadowAttachments =  {
			mMainLight.cascades[i].imageView
		};
		mMainLight.cascades[i].framebuffer = pCore->CreateFramebuffer( mShadowPass, shadowAttachments, mMainLight.shadowImage.width, mMainLight.shadowImage.height);

		mDeletionStack.Push( [&, i](){
			pCore->Device().destroyFramebuffer( mMainLight.cascades[i].framebuffer);
			pCore->Device().destroyImageView( mMainLight.cascades[i].imageView);
		});
	}
}


void Renderer::CreateRenderPasses() {

	// attachements
	auto presentAttachment = vk::AttachmentDescription{}
		.setFormat( mSwapchain.imageFormat )
		.setSamples( vk::SampleCountFlagBits::e1 )
		.setLoadOp( vk::AttachmentLoadOp::eClear )
		.setStoreOp( vk::AttachmentStoreOp::eStore )
		.setStencilLoadOp( vk::AttachmentLoadOp::eClear )
		.setStencilStoreOp( vk::AttachmentStoreOp::eDontCare )
		.setInitialLayout( vk::ImageLayout::eUndefined )
		.setFinalLayout( vk::ImageLayout::ePresentSrcKHR );
		
	auto depthAttachment = vk::AttachmentDescription{}
		.setFormat( mDepthFormat )
		.setSamples( vk::SampleCountFlagBits::e1 )
		.setLoadOp( vk::AttachmentLoadOp::eClear )
		.setStoreOp( vk::AttachmentStoreOp::eStore )
		.setStencilLoadOp( vk::AttachmentLoadOp::eClear )
		.setStencilStoreOp( vk::AttachmentStoreOp::eDontCare )
		.setInitialLayout( vk::ImageLayout::eUndefined )
		.setFinalLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );
		
	auto shadowAttachment = vk::AttachmentDescription{}
		.setFormat( mDepthFormat )
		.setSamples( vk::SampleCountFlagBits::e1 )
		.setLoadOp( vk::AttachmentLoadOp::eClear )
		.setStoreOp( vk::AttachmentStoreOp::eStore )
		.setStencilLoadOp( vk::AttachmentLoadOp::eClear )
		.setStencilStoreOp( vk::AttachmentStoreOp::eDontCare )
		.setInitialLayout( vk::ImageLayout::eUndefined )
		.setFinalLayout( vk::ImageLayout::eShaderReadOnlyOptimal );

	// TODO resolve attachment if MSAA is enabled ( MsaaSamples > 1)

	// forward pass
	Array<vk::AttachmentDescription> forwardAttachments = {
		presentAttachment, depthAttachment
	};
	mForwardPass = pCore->CreateRenderPass( forwardAttachments, 1);

	mDeletionStack.Push( [&](){
		pCore->Device().destroyRenderPass( mForwardPass);
	});

	// shadow pass
	Array<vk::AttachmentDescription> shadowAttachments = {
		shadowAttachment
	};
	mShadowPass = pCore->CreateRenderPass( shadowAttachments, 0);

	mDeletionStack.Push( [&](){
		pCore->Device().destroyRenderPass( mShadowPass);
	});
}


void Renderer::CreateFramebuffers() {

	mForwardFramebuffers.Resize( mSwapchain.imageCount);
	mShadowFramebuffers.Resize( mSwapchain.imageCount);
	for (U32 i = 0; i < mSwapchain.imageCount; i++)
	{
		// forward pass
		Array<vk::ImageView> forwardAttachments =  {
			mSwapchain.imageViews[i], mDepthImage.imageView
		};
		mForwardFramebuffers[i] = pCore->CreateFramebuffer( mForwardPass, forwardAttachments, mSwapchain.extent.width, mSwapchain.extent.height);

		// shadow pass
		Array<vk::ImageView> shadowAttachments =  {
			mShadowImage.imageView
		};
		mShadowFramebuffers[i] = pCore->CreateFramebuffer( mShadowPass, shadowAttachments, mShadowImage.width, mShadowImage.height);
	}
}


void Renderer::DestroyFramebuffers() {

	// forward pass
	for (auto &framebuffer : mForwardFramebuffers)
	{
		pCore->Device().destroyFramebuffer( framebuffer);
	}	
	// shadow pass
	for (auto &framebuffer : mShadowFramebuffers)
	{
		pCore->Device().destroyFramebuffer( framebuffer);
	}
}


void Renderer::RecreateSwapchain() {

	pCore->Device().waitIdle();

	// cleanup old objects	
	// framebuffers
	DestroyFramebuffers();

	// depth image
	pCore->Device().destroyImageView( mDepthImage.imageView);
	pCore->Device().destroyImage( mDepthImage.image);
	pCore->Device().freeMemory( mDepthImage.imageMemory);

	// depth pyramid here
	for (auto imageView : mDepthPyramidViews)
	{
		pCore->Device().destroyImageView( imageView);
	}
	pCore->Device().destroyImageView( mDepthPyramid.imageView);
	pCore->Device().destroyImage( mDepthPyramid.image);
	pCore->Device().freeMemory( mDepthPyramid.imageMemory);


	// swapchain
	for(auto &imageView : mSwapchain.imageViews) 
	{
	    pCore->Device().destroyImageView( imageView, nullptr);
	}

	vk::SwapchainKHR oldSwapchain = mSwapchain.swapchain;
	pCore->CreateSwapchain(mSwapchain);
	pCore->Device().destroySwapchainKHR( oldSwapchain, nullptr);
	
	// recreate presentation objects
	CreateDepthResources();
	CreateFramebuffers();
}


void Renderer::RegisterMeshObject( MeshObject *object) {

	RenderObject newObject;
	newObject.transform = &object->transform;
	newObject.sphereBounds = &object->sphereBounds;
	newObject.meshId = RegisterMesh( object->meshName);
	newObject.materialId = RegisterMaterial( object->materialName);
	newObject.updated = true;
	newObject.passIndex.Clear( -1);

	U32 objIdx;
	if ( mReuseObjectIndices.IsEmpty())
	{
		objIdx = (U32)mRenderObjects.GetSize();
		mRenderObjects.PushBack( newObject);
	}
	else
	{
		objIdx = mReuseObjectIndices.Back();
		mReuseObjectIndices.PopBack();
		mRenderObjects[ objIdx] = newObject;
	}
	mDirtyObjectIndices.PushBack( objIdx);

	object->objectIdx = objIdx;

	// submit to mesh passes
	Material &material = mMaterials[ newObject.materialId];
	if ( material.usedInPass[PassType::SHADOW] )
	{
		mShadowMeshPass.unbatchedIndices.PushBack( objIdx);
	}
	if ( material.usedInPass[PassType::OPAQUE] )
	{
		mOpaqueMeshPass.unbatchedIndices.PushBack( objIdx);
	}
	if ( material.usedInPass[PassType::TRANSPARENT] )
	{
		mTransparentMeshPass.unbatchedIndices.PushBack( objIdx);
	}
}

    
void Renderer::DeleteMeshObject( MeshObject *object) {

	auto &deleteObj = mRenderObjects[ object->objectIdx];

	if ( deleteObj.passIndex[ PassType::SHADOW] >= 0) 
	{
		mShadowMeshPass.deleteObjectIndices.PushBack( (U32)deleteObj.passIndex[ PassType::SHADOW]);
	}
	if ( deleteObj.passIndex[ PassType::OPAQUE] >= 0) 
	{
		mOpaqueMeshPass.deleteObjectIndices.PushBack( (U32)deleteObj.passIndex[ PassType::OPAQUE]);
	}
	if ( deleteObj.passIndex[ PassType::TRANSPARENT] >= 0) 
	{
		mTransparentMeshPass.deleteObjectIndices.PushBack( (U32)deleteObj.passIndex[ PassType::TRANSPARENT]);
	}

	mReuseObjectIndices.PushBack( object->objectIdx);
}


void Renderer::UpdateMeshObject( MeshObject *object) {

	if ( !mRenderObjects[ object->objectIdx].updated)
	{
		mDirtyObjectIndices.PushBack( object->objectIdx);
	}	
}


U64 Renderer::RegisterMesh( std::string_view meshName) {

	U64 meshId = SID( meshName.data());
	if ( mMeshes.Find( meshId) == mMeshes.End())
	{
		CreateMesh( meshName);
		mMeshesDirty = true;
		// TODO catch fail state if mesh file does not exist -> log warning and return default/error mesh ID
	}

	return meshId;
}


U64 Renderer::RegisterMaterial( std::string_view materialName) {

	U64 materialId = SID( materialName.data());
	if ( mMaterials.Find( materialId) == mMaterials.End())
	{
		CreateMaterial( materialName);
		// TODO catch fail state if material file does not exist -> log warning and return default/error material ID
	}

	return materialId;
}


void Renderer::CreateMesh( std::string_view meshName) {

	U64 meshId = SID( meshName.data());
	Mesh newMesh;

	newMesh.meshData = pResources->GetMesh( mResourceDir + meshName.data());
	newMesh.vertexCount = (U32)newMesh.meshData->vertices.GetSize();
	newMesh.indexCount = (U32)newMesh.meshData->indices.GetSize();

	newMesh.meshName = meshName;
	mMeshes[ meshId] = newMesh;
}


void Renderer::CreateMaterial( std::string_view materialName) {

	U64 materialId = SID( materialName.data());
	Material newMaterial;

	// read material json
	Array<char> content;
	mFiles.Read( mResourceDir + materialName.data(), content);
	Json materialJson = Json::Load( std::string_view( content.Data(), content.GetSize()));

	// get pipeline
	std::string pipelineName = materialJson.At( "pipeline").ToString();
	U64 pipelineId = SID( pipelineName.c_str());
	if ( mPipelines.Find( pipelineId) == mPipelines.End())
	{
		CreatePipeline( pipelineName, mForwardPass);
	}

	// get images and build descriptor sets
	auto &textures = materialJson.At( "textures").GetList();
	DescriptorSetBuilder builder( pCore->Device(), pDescriptorSetAllocator, pDescriptorLayoutCache);
	U32 binding = 0;
	Array<vk::DescriptorImageInfo> imageInfos;
	imageInfos.Reserve( textures.GetSize());
	for ( auto &texture : textures) 
	{
		//texture format
		vk::Format format = JsonVk::GetFormat( texture.At( "format").ToString());

		// texture image
		std::string imageName = texture.At( "image").ToString();
		U64 imageId = SID( imageName.c_str());
		if ( mTextures.Find( imageId) == mTextures.End())
		{
			CreateTexture( imageName, format);
		}

		// texture sampler
		std::string samplerName = texture.At( "sampler").ToString();
		U64 samplerId = SID( samplerName.c_str());
		if ( mSamplers.Find( samplerId) == mSamplers.End())
		{
			CreateSampler( samplerName);
		}

		imageInfos.PushBack( mTextures[ imageId].DescriptorInfo( mSamplers[ samplerId]));
		builder.BindImage(binding, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, &imageInfos.Back());
		++binding;
	}
	vk::DescriptorSet descriptorSet = builder.Build();

	// set pass data
	newMaterial.usedInPass.Clear( false);
	if ( materialJson.At( "shadow").ToBool())
	{
		newMaterial.usedInPass[PassType::SHADOW] = true;
		newMaterial.pipelineId[PassType::SHADOW] = SID( "Pipelines/default_shadow.pipeline.json");
		newMaterial.descriptorSet[PassType::SHADOW] = vk::DescriptorSet{};
	}
	if ( materialJson.At( "transparent").ToBool())
	{
		newMaterial.usedInPass[PassType::TRANSPARENT] = true;
		newMaterial.pipelineId[PassType::TRANSPARENT] = pipelineId;
		newMaterial.descriptorSet[PassType::TRANSPARENT] = descriptorSet;
	}
	else
	{
		newMaterial.usedInPass[PassType::OPAQUE] = true;
		newMaterial.pipelineId[PassType::OPAQUE] = pipelineId;
		newMaterial.descriptorSet[PassType::OPAQUE] = descriptorSet;
	}

	newMaterial.materialName = materialName;
	mMaterials[ materialId] = newMaterial;
}


void Renderer::CreateTexture( std::string_view textureName, vk::Format format) {

	U64 imageId = SID( textureName.data());
	Image image;

	// load image data
	ImageData *imageData = pResources->GetImage( mResourceDir + textureName.data());

	// create staging buffer
	Size imageSize = imageData->pixelData.GetSize();
	Buffer stagingBuffer = CreateStagingBuffer( imageSize);

	// transfer image data to staging buffer
	void *data;
	vk::Result result = pCore->Device().mapMemory( stagingBuffer.bufferMemory, 0, imageSize, vk::MemoryMapFlags(), &data);
	if( result == vk::Result::eSuccess ) 
	{
		memcpy(data, imageData->pixelData.Data(), imageSize);
		pCore->Device().unmapMemory( stagingBuffer.bufferMemory);
	}
	else 
	{
		throw std::runtime_error("Failed to map image staging buffer memory " + vk::to_string(result));
	}

	// create image on device memory
	image.width = imageData->width;
	image.height = imageData->height;
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

	// add to cache
	mTextures[ imageId] = image;

	//cleanup
	pCore->Device().destroyBuffer( stagingBuffer.buffer, nullptr);
	pCore->Device().freeMemory( stagingBuffer.bufferMemory, nullptr);

	mDeletionStack.Push( [&, imageId](){
		pCore->Device().destroyImageView( mTextures[ imageId].imageView);
		pCore->Device().destroyImage( mTextures[ imageId].image);
		pCore->Device().freeMemory( mTextures[ imageId].imageMemory);
	});
}


void Renderer::CreateSampler( std::string_view samplerName) {

	U64 samplerId = SID( samplerName.data());

	// read sampler json
	Array<char> content;
	mFiles.Read( mResourceDir + samplerName.data(), content);
	Json samplerJson = Json::Load( std::string_view( content.Data(), content.GetSize()));

	auto samplerInfo = vk::SamplerCreateInfo{}
		.setMinFilter( JsonVk::GetFilter( samplerJson.At( "minFilter").ToString()) )
		.setMagFilter( JsonVk::GetFilter( samplerJson.At( "magFilter").ToString()) )
		.setAddressModeU( JsonVk::GetAddressMode( samplerJson.At( "addressModeU").ToString()) )
		.setAddressModeV( JsonVk::GetAddressMode( samplerJson.At( "addressModeV").ToString()) )
		.setAddressModeW( JsonVk::GetAddressMode( samplerJson.At( "addressModeW").ToString()) )
		.setAnisotropyEnable( samplerJson.At( "enableAnisotropy").ToBool() )
		.setMaxAnisotropy( (float)pMaxAnisotropy->Get() )
		.setMipmapMode( JsonVk::GetMipMapMode( samplerJson.At( "mipmapMode").ToString()) )
		.setMinLod( (float)samplerJson.At( "minLod").ToFloat() )
		.setMaxLod( (float)samplerJson.At( "maxLod").ToFloat() )
		.setMipLodBias( (float)samplerJson.At( "mipLodBias").ToFloat() )
		.setCompareEnable( samplerJson.At( "enableCompare").ToBool() )
		.setCompareOp( JsonVk::GetCompareOp( samplerJson.At( "compareOp").ToString()) )
		.setBorderColor( JsonVk::GetBorder( samplerJson.At( "borderColor").ToString()) );
	
	vk::SamplerReductionModeCreateInfo reductionModeInfo;
	if ( !samplerJson["reductionMode"].IsNull() )
	{
		reductionModeInfo.setReductionMode( JsonVk::GetReductionMode( samplerJson[ "reductionMode"].ToString()) );
		samplerInfo.setPNext( &reductionModeInfo);
	}	

	vk::Sampler newSampler;
	vk::Result result = pCore->Device().createSampler(&samplerInfo, nullptr, &newSampler);
	if( result != vk::Result::eSuccess )
	{
		throw std::runtime_error("Failed to create texture sampler " + vk::to_string(result));
	}

	mSamplers[ samplerId] = newSampler;

	mDeletionStack.Push( [&, samplerId](){
		pCore->Device().destroySampler( mSamplers[ samplerId]);
	});
}


void Renderer::CreatePipeline( std::string_view pipelineName, vk::RenderPass renderPass) {

	U64 pipelineId = SID( pipelineName.data());

	// read pipeline json
	Array<char> content;
	mFiles.Read( mResourceDir + pipelineName.data(), content);
	Json pipelineJson = Json::Load( std::string_view( content.Data(), content.GetSize()));

	// read descriptor set layouts
	Array<vk::DescriptorSetLayout> layouts;
	auto &descriptorSetLayoutsJson = pipelineJson.At( "descriptorSetLayouts");
	for ( auto &setBindingsJson : descriptorSetLayoutsJson.GetList())
	{
		Array<vk::DescriptorSetLayoutBinding> layoutBindings;
		for( auto &bindingJson : setBindingsJson.GetList())
		{
			U32 binding = (U32)bindingJson.At( "binding").ToInt();
			vk::DescriptorType type = JsonVk::GetDescriptorType( bindingJson.At( "type").ToString());

			vk::ShaderStageFlags stages;
			auto &stageNames = bindingJson.At( "stages").GetList();
			for ( auto &stage : stageNames)
			{
				stages |= JsonVk::GetShaderStage( stage.ToString());
			}

			layoutBindings.PushBack(
				vk::DescriptorSetLayoutBinding{}
					.setBinding( binding )
					.setDescriptorCount( 1 )
					.setDescriptorType( type )
					.setStageFlags( stages )
			);
		}

		auto layoutInfo = vk::DescriptorSetLayoutCreateInfo{}
			.setBindingCount( (U32)layoutBindings.GetSize() )
			.setPBindings( layoutBindings.Data() );

		layouts.PushBack( pDescriptorLayoutCache->CreateLayout( &layoutInfo));
	}


	// read push constant ranges (optional)
	Array<vk::PushConstantRange> pushConstants;
	auto &pushConstantsJson = pipelineJson[ "pushConstants"];

	if ( !pushConstantsJson.IsNull())
	{
		for ( auto &pcJson : pushConstantsJson.GetList())
		{
			U32 size = (U32)pcJson.At( "size").ToInt();
			U32 offset = (U32)pcJson.At( "offset").ToInt();
			vk::ShaderStageFlags stages;
			auto &stageNames = pcJson.At( "stages").GetList();
			for ( auto &stage : stageNames)
			{
				stages |= JsonVk::GetShaderStage( stage.ToString());
			}

			auto pushConstantRange = vk::PushConstantRange{}
				.setSize( size )
				.setOffset( offset )
				.setStageFlags( stages );

			pushConstants.PushBack( pushConstantRange);
		}
	}

	// read shader stages
	auto shaderStages = pipelineJson.At( "shaders").GetDict();
	for ( auto &[stage, shaderName] : shaderStages)
	{
		U64 shaderId = SID( shaderName.ToString().c_str());
		if ( mShaders.Find( shaderId) == mShaders.End())
		{
			CreateShaderModule( shaderName.ToString());
		}
	}

	// build pipeline
	Pipeline newPipeline;
	std::string type = pipelineJson.At( "type").ToString();
	if ( type == "graphics")
	{
		GraphicsPipelineBuilder pipelineBuilder = mDefaultPipelineBuilder;

		pipelineBuilder.descriptorLayouts = layouts;
		pipelineBuilder.pushConstants = pushConstants;

		// shader stage infos
		// vertex shader mandatory
		if ( !shaderStages[ "vertex"].IsNull())
		{
			pipelineBuilder.shaderInfos.PushBack( 
				vk::PipelineShaderStageCreateInfo{}
					.setStage( vk::ShaderStageFlagBits::eVertex )
					.setModule( mShaders[ SID( shaderStages.At( "vertex").ToString().c_str())] )
					.setPName( "main" )
			);
		}
		else
		{
			mLog.Error( LogChannel::GRAPHICS, FormatStr("No vertex shader provided for graphics pipeline \"%s\".", pipelineName));
		} 
		// fragment shader optional
		if ( !shaderStages[ "fragment"].IsNull())
		{
			pipelineBuilder.shaderInfos.PushBack( 
				vk::PipelineShaderStageCreateInfo{}
					.setStage( vk::ShaderStageFlagBits::eFragment )
					.setModule( mShaders[ SID( shaderStages.At( "fragment").ToString().c_str())] )
					.setPName( "main" )
			);
		}
		// geometry shader optional
		if ( !shaderStages[ "geometry"].IsNull())
		{
			pipelineBuilder.shaderInfos.PushBack( 
				vk::PipelineShaderStageCreateInfo{}
					.setStage( vk::ShaderStageFlagBits::eGeometry )
					.setModule( mShaders[ SID( shaderStages.At( "geometry").ToString().c_str())] )
					.setPName( "main" )
			);
		}
		// tesselation shaders optional but control requires eval shader
		if ( !shaderStages[ "tesselation_eval"].IsNull())
		{
			pipelineBuilder.shaderInfos.PushBack( 
				vk::PipelineShaderStageCreateInfo{}
					.setStage( vk::ShaderStageFlagBits::eTessellationEvaluation )
					.setModule( mShaders[ SID( shaderStages.At( "tesselation_eval").ToString().c_str())] )
					.setPName( "main" )
			);
			if ( !shaderStages[ "tesselation_ctrl"].IsNull())
			{
				pipelineBuilder.shaderInfos.PushBack( 
					vk::PipelineShaderStageCreateInfo{}
						.setStage( vk::ShaderStageFlagBits::eTessellationControl )
						.setModule( mShaders[ SID( shaderStages.At( "tesselation_ctrl").ToString().c_str())] )
						.setPName( "main" )
				);				
			}
		}
		
		// read fixed stage configuration from json
		pipelineBuilder.FillFromJson( pipelineJson);

		newPipeline = pipelineBuilder.Build( pCore->Device(), renderPass);
	}
	else if ( type == "compute")
	{
		U64 shaderId = SID( shaderStages.At( "compute").ToString().c_str());

		ComputePipelineBuilder pipelineBuilder;
		pipelineBuilder.descriptorLayouts = layouts;
		pipelineBuilder.pushConstants = pushConstants;
		pipelineBuilder.shaderInfo
			.setStage( vk::ShaderStageFlagBits::eCompute )
			.setModule( mShaders[ shaderId] )
			.setPName( "main" );

		newPipeline = pipelineBuilder.Build( pCore->Device());
	}
	else 
	{
		mLog.Error( LogChannel::GRAPHICS, FormatStr("Unknown pipeline type \"%s\" in \"%s\".", type, pipelineName));
	}

	mPipelines[ pipelineId] = newPipeline;

	mDeletionStack.Push( [&, pipelineId](){
		pCore->Device().destroyPipeline( mPipelines[ pipelineId].pipeline);
		pCore->Device().destroyPipelineLayout( mPipelines[ pipelineId].pipelineLayout);
	});
}


void Renderer::CreateShaderModule( std::string_view shaderName) {

	U64 shaderId = SID( shaderName.data());

	Array<char> shaderCode;
	mFiles.Read( mResourceDir + shaderName.data(), shaderCode, std::ios::binary);
	auto newShader = pCore->CreateShaderModule( shaderCode.GetSize(), shaderCode.Data() );

	mShaders[ shaderId] = newShader;

	mDeletionStack.Push( [&, shaderId](){
		pCore->Device().destroyShaderModule( mShaders[ shaderId]);
	});
}


void Renderer::MergeMeshes() {

	// assign meshes their proper position in the vertex and index arrays
	U32 totalVertices = 0;
	U32 totalIndices = 0;
	for ( auto &[meshId, mesh] : mMeshes)
	{
		mesh.firstVertex = totalVertices;
		mesh.firstIndex = totalIndices;
		
		totalVertices += mesh.vertexCount;
		totalIndices += mesh.indexCount;
	}


	// recreate vertex and index buffers if they are too small
	if( mCombinedVertexBuffer.bufferSize < totalVertices * sizeof( Vertex))
	{
		CreateBuffer( 
			mCombinedVertexBuffer,
			totalVertices * sizeof(Vertex), 
			vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
	}
	if( mCombinedIndexBuffer.bufferSize < totalIndices * sizeof( U32))
	{
		CreateBuffer( 
			mCombinedIndexBuffer,
			totalIndices * sizeof(U32), 
			vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
	}

	// fill staging buffers
	Buffer stagingVertexBuffer = CreateStagingBuffer( totalVertices * sizeof(Vertex));
	void *vertexData;
	vk::Result result = pCore->Device().mapMemory( stagingVertexBuffer.bufferMemory, 0, stagingVertexBuffer.bufferSize, vk::MemoryMapFlags(), &vertexData);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Failed to map staging buffer memory " + vk::to_string(result));
	}

	Buffer stagingIndexBuffer = CreateStagingBuffer( totalIndices * sizeof(U32));
	void *indexData;
	result = pCore->Device().mapMemory( stagingIndexBuffer.bufferMemory, 0, stagingIndexBuffer.bufferSize, vk::MemoryMapFlags(), &indexData);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Failed to map staging buffer memory " + vk::to_string(result));
	}

	for ( auto &[meshId, mesh] : mMeshes)
	{
		memcpy( (Byte*)vertexData + mesh.firstVertex * sizeof(Vertex), mesh.meshData->vertices.Data(), mesh.vertexCount * sizeof(Vertex));
		memcpy( (Byte*)indexData + mesh.firstIndex * sizeof(U32), mesh.meshData->indices.Data(), mesh.indexCount * sizeof(U32));
	}

	pCore->Device().unmapMemory( stagingVertexBuffer.bufferMemory);
	pCore->Device().unmapMemory( stagingIndexBuffer.bufferMemory);

	// upload buffer data to Gpu memory
	CopyBuffer( stagingVertexBuffer.buffer, mCombinedVertexBuffer.buffer, 0, stagingVertexBuffer.bufferSize);
	CopyBuffer( stagingIndexBuffer.buffer, mCombinedIndexBuffer.buffer, 0, stagingIndexBuffer.bufferSize);

	// cleanup
	pCore->Device().destroyBuffer( stagingVertexBuffer.buffer);
	pCore->Device().freeMemory( stagingVertexBuffer.bufferMemory);
	pCore->Device().destroyBuffer( stagingIndexBuffer.buffer);
	pCore->Device().freeMemory( stagingIndexBuffer.bufferMemory);
}


void Renderer::UpdateMeshPass( MeshPass *pass) {

	// TODO use better sortKey including the materials pipeline

	// handle deleted render objects
	if ( !pass->deleteObjectIndices.IsEmpty())
	{
		// mLog.Info( LogChannel::GRAPHICS, FormatStr( "Deleting %d objects from mesh pass %d.", pass->deleteObjectIndices.GetSize(), (U8)pass->passType));

		// render batches of deleted objects
		Array<RenderBatch> deleteBatches;
		deleteBatches.Reserve( pass->deleteObjectIndices.GetSize());
		for ( U32 idx : pass->deleteObjectIndices)
		{
			pass->reuseObjectIndices.PushBack( idx);
			RenderObject &obj = mRenderObjects[ pass->objectIndices[ idx]];

			RenderBatch newBatch;
			newBatch.objectIdx = pass->objectIndices[ idx];
			newBatch.sortKey = (U64)obj.materialId << 32 | obj.meshId;

			deleteBatches.PushBack( newBatch);
		}
		pass->deleteObjectIndices.Clear();

		// remove deleted batches from renderBatches array
		std::sort( deleteBatches.Begin(), deleteBatches.End());
		std::set_difference( pass->renderBatches.Begin(), pass->renderBatches.End(), deleteBatches.Begin(), deleteBatches.End(), pass->renderBatches.Begin());
		pass->renderBatches.Resize( pass->renderBatches.GetSize() - deleteBatches.GetSize());

		pass->hasChanged = true;
	}


	// handle unbatched objects
	if ( !pass->unbatchedIndices.IsEmpty())
	{
		// mLog.Info( LogChannel::GRAPHICS, FormatStr( "Adding %d objects to mesh pass %d.", pass->unbatchedIndices.GetSize(), (U8)pass->passType));

		Array<RenderBatch> newBatches;
		newBatches.Reserve( pass->unbatchedIndices.GetSize());
		for ( U32 objIdx : pass->unbatchedIndices)
		{
			// insert new render object and create renderbatch for it
			U32 newIndex;
			if ( !pass->reuseObjectIndices.IsEmpty())
			{
				newIndex = pass->reuseObjectIndices.Back();
				pass->reuseObjectIndices.PopBack();
				pass->objectIndices[ newIndex] = objIdx;
			}
			else
			{
				newIndex = (U32)pass->objectIndices.GetSize();
				pass->objectIndices.PushBack( objIdx);
			}

			RenderObject &obj = mRenderObjects[ objIdx];
			obj.passIndex[ pass->passType] = newIndex;

			RenderBatch newBatch;
			newBatch.objectIdx = objIdx;
			newBatch.sortKey = (U64)obj.materialId << 32 | obj.meshId;
			newBatches.PushBack( newBatch);
		}
		pass->unbatchedIndices.Clear();

		// combine new batches with renderBatches and sort
		std::sort( newBatches.Begin(), newBatches.End());
		if ( pass->renderBatches.IsEmpty())
		{
			pass->renderBatches = std::move( newBatches);
		}
		else 
		{
			Size oldSize = pass->renderBatches.GetSize();
			Size newSize = oldSize + newBatches.GetSize();
			pass->renderBatches.Reserve( newSize);
			for ( auto &batch : newBatches)
			{
				pass->renderBatches.PushBack( batch);
			}

			auto begin = pass->renderBatches.Begin();
			auto middle = begin + oldSize;
			auto end = begin + newSize;
			std::inplace_merge( begin, middle, end);
		}

		pass->hasChanged = true;
	}

	// if nothing changed no batch updates needed
	if ( !pass->hasChanged)
	{
		return;
	}

	// rebuild indirect batches
	BuildMeshPassBatches(pass);

	// resize Gpu side buffers if necessary
	if ( pass->drawIndirectBuffer.bufferSize < pass->indirectBatches.GetSize() * sizeof(IndirectData))
	{
		CreateBuffer(
			pass->drawIndirectBuffer,
			pass->indirectBatches.GetSize() * sizeof(IndirectData),
			vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eIndirectBuffer | vk::BufferUsageFlagBits::eTransferDst,
			// vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent 
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
	}
	if ( pass->instanceDataBuffer.bufferSize < pass->renderBatches.GetSize() * sizeof(InstanceData))
	{
		CreateBuffer(
			pass->instanceDataBuffer,
			pass->renderBatches.GetSize() * sizeof(InstanceData),
			vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
	}
	if ( pass->instanceIndexBuffer.bufferSize < pass->renderBatches.GetSize() * sizeof(U32))
	{
		CreateBuffer(
			pass->instanceIndexBuffer,
			pass->renderBatches.GetSize() * sizeof(U32),
			vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
			// vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent 
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
	}

	pass->hasChanged = false;
}


void Renderer::BuildMeshPassBatches( MeshPass *pass) {

	if ( pass->renderBatches.IsEmpty())
	{
		return;
	}

	// rebuild indirect batches
	pass->indirectBatches.Clear();
	pass->multiBatches.Clear();

	IndirectBatch newBatch;
	newBatch.first = 0;
	newBatch.count = 0;
	newBatch.objectIdx = pass->renderBatches[0].objectIdx;
	
	MultiBatch newMultiBatch;
	newMultiBatch.first = 0;
	newMultiBatch.count = 1;

	pass->indirectBatches.PushBack( newBatch);
	pass->multiBatches.PushBack( newMultiBatch);

	U64 lastMaterial = mRenderObjects[ newBatch.objectIdx].materialId;
	U64 lastMesh = mRenderObjects[ newBatch.objectIdx].meshId;

	U32 numBatches = (U32)pass->renderBatches.GetSize();
	for ( U32 idx = 0; idx < numBatches; idx++)
	{
		RenderObject &obj = mRenderObjects[ pass->renderBatches[idx].objectIdx ];

		if ( obj.materialId != lastMaterial || obj.meshId != lastMesh)
		{
			newBatch.first = idx;
			newBatch.count = 1;
			newBatch.objectIdx = pass->renderBatches[idx].objectIdx;
			pass->indirectBatches.PushBack( newBatch);

			if ( obj.materialId != lastMaterial && pass->passType != PassType::SHADOW )  // single multibatch for shadow pass
			{
				newMultiBatch.first = (U32)pass->indirectBatches.GetSize();
				newMultiBatch.count = 1;
				pass->multiBatches.PushBack( newMultiBatch);

				lastMaterial = obj.materialId;
			}
			else 
			{
				pass->multiBatches.Back().count++;
			}

			if ( obj.meshId != lastMesh)
			{
				lastMesh = obj.meshId;
			}
			
		}
		else 
		{
			pass->indirectBatches.Back().count++;
		}
	}
}


void Renderer::UpdateMeshPassBatchBuffer( MeshPass *pass, vk::CommandBuffer cmd) {

	if ( pass->indirectBatches.IsEmpty())
	{
		return;
	}

	Size copySize = pass->indirectBatches.GetSize() * sizeof(IndirectData);

	// fill staging buffer
	Buffer stagingBuffer = CreateStagingBuffer( copySize);
	void *data;
	vk::Result result = pCore->Device().mapMemory( stagingBuffer.bufferMemory, 0, copySize, vk::MemoryMapFlags(), &data);
	if( result != vk::Result::eSuccess )
	{
		throw std::runtime_error("Failed to map staging buffer memory " + vk::to_string(result));
	}

	IndirectData *indirectData = (IndirectData*)data;
	for ( U32 i = 0; i < pass->indirectBatches.GetSize(); i++)
	{
		IndirectBatch batch = pass->indirectBatches[i];
		Mesh &mesh = mMeshes[ mRenderObjects[batch.objectIdx].meshId ];

		indirectData[i].batchIdx = i;
		indirectData[i].objectIdx = batch.objectIdx;
		indirectData[i].drawIndirectCmd
			.setVertexOffset( mesh.firstVertex )
			.setFirstIndex( mesh.firstIndex )
			.setIndexCount( mesh.indexCount )
			.setFirstInstance( batch.first )
			.setInstanceCount( 0 );
			// .setInstanceCount( batch.count );
	}
	pCore->Device().unmapMemory( stagingBuffer.bufferMemory);

	auto copyRegion = vk::BufferCopy{}
		.setSrcOffset( 0 )
		.setDstOffset( 0 )
		.setSize( copySize );
		
	cmd.copyBuffer(stagingBuffer.buffer, pass->drawIndirectBuffer.buffer, 1, &copyRegion);

	{
		std::lock_guard<std::mutex> lock( mUpdateMutex);

		mPreCullBarriers.PushBack( 
			vk::BufferMemoryBarrier{}
				.setBuffer( pass->drawIndirectBuffer.buffer)
				.setSize( VK_WHOLE_SIZE)
				.setSrcAccessMask( vk::AccessFlagBits::eTransferWrite)
				.setDstAccessMask( vk::AccessFlagBits::eShaderRead)
				.setSrcQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
				.setDstQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
		);

		// cleanup
		CurrentFrame().deletionStack.Push( [&, stagingBuffer]() {

			pCore->Device().destroyBuffer( stagingBuffer.buffer);
			pCore->Device().freeMemory( stagingBuffer.bufferMemory);
		});
	}
}
    
	
void Renderer::UpdateMeshPassInstanceBuffer( MeshPass *pass, vk::CommandBuffer cmd) {

	if ( pass->indirectBatches.IsEmpty())
	{
		return;
	}

	Size copySize = pass->renderBatches.GetSize() * sizeof(InstanceData);

	// fill staging buffer
	Buffer stagingBuffer = CreateStagingBuffer( copySize);
	void *data;
	vk::Result result = pCore->Device().mapMemory( stagingBuffer.bufferMemory, 0, copySize, vk::MemoryMapFlags(), &data);
	if( result != vk::Result::eSuccess )
	{
		throw std::runtime_error("Failed to map staging buffer memory " + vk::to_string(result));
	}

	InstanceData *instanceData = (InstanceData*)data;
	for ( U32 batchIdx = 0; batchIdx < pass->indirectBatches.GetSize(); batchIdx++)
	{
		IndirectBatch batch = pass->indirectBatches[ batchIdx];
		for ( U32 i = 0; i < batch.count; i++)
		{
			instanceData[ batch.first + i].batchIdx = batchIdx;
			instanceData[ batch.first + i].objectIdx = pass->renderBatches[ batch.first + i].objectIdx;
		}
	}
	pCore->Device().unmapMemory( stagingBuffer.bufferMemory);

	auto copyRegion = vk::BufferCopy{}
		.setSrcOffset( 0 )
		.setDstOffset( 0 )
		.setSize( copySize );
		
	cmd.copyBuffer(stagingBuffer.buffer, pass->instanceDataBuffer.buffer, 1, &copyRegion);

	{
		std::lock_guard<std::mutex> lock( mUpdateMutex);

		mPreCullBarriers.PushBack( 
			vk::BufferMemoryBarrier{}
				.setBuffer( pass->instanceDataBuffer.buffer)
				.setSize( VK_WHOLE_SIZE)
				.setSrcAccessMask( vk::AccessFlagBits::eTransferWrite)
				.setDstAccessMask( vk::AccessFlagBits::eShaderRead)
				.setSrcQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
				.setDstQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
		);

		// cleanup
		CurrentFrame().deletionStack.Push( [&, stagingBuffer]() {

			pCore->Device().destroyBuffer( stagingBuffer.buffer);
			pCore->Device().freeMemory( stagingBuffer.bufferMemory);
		});
	}
}


void Renderer::UpdateObjectBuffer( vk::CommandBuffer cmd) {

	// do nothing if object data has not changed
	if ( mDirtyObjectIndices.IsEmpty())
	{
		return;
	}

	Size copySize = mRenderObjects.GetSize() * sizeof(ObjectData);

	// recreate object buffer if necessary
	if ( mObjectBuffer.bufferSize < copySize)
	{
		CreateBuffer( 
			mObjectBuffer, copySize,
			vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
	}

	// // reupload whole object data buffer
	// Buffer stagingBuffer = CreateStagingBuffer( copySize);
	// void *data;
	// vk::Result result = pCore->Device().mapMemory( stagingBuffer.bufferMemory, 0, copySize, vk::MemoryMapFlags(), &data);
	// if( result != vk::Result::eSuccess )
	// {
	// 	throw std::runtime_error("Failed to map staging buffer memory " + vk::to_string(result));
	// }

	// ObjectData *objData = (ObjectData*)data;
	// for ( Size i = 0; i < mRenderObjects.GetSize(); i++)
	// {
	// 	objData[i].transform = *mRenderObjects[i].transform;
	// 	objData[i].sphereBounds = *mRenderObjects[i].sphereBounds;
	// 	mRenderObjects[i].updated = false;
	// }
	// pCore->Device().unmapMemory( stagingBuffer.bufferMemory);

	// auto copyRegion = vk::BufferCopy{}
	// 	.setSrcOffset( 0 )
	// 	.setDstOffset( 0 )
	// 	.setSize( copySize );
			
	// cmd.copyBuffer(stagingBuffer.buffer, mObjectBuffer.buffer, 1, &copyRegion);


	// TODO check if compute shader performs better
	// partial buffer update
	Buffer stagingBuffer = CreateStagingBuffer( mDirtyObjectIndices.GetSize() * sizeof( ObjectData));
	void *data;
	vk::Result result = pCore->Device().mapMemory( stagingBuffer.bufferMemory, 0, VK_WHOLE_SIZE, vk::MemoryMapFlags(), &data);
	if( result != vk::Result::eSuccess )
	{
		throw std::runtime_error("Failed to map staging buffer memory " + vk::to_string(result));
	}
	ObjectData *objData = (ObjectData*)data;

	Array<vk::BufferCopy> copies;
	copies.Reserve( mDirtyObjectIndices.GetSize());
	for( U32 i = 0; i < (U32)mDirtyObjectIndices.GetSize(); i++)
	{
		objData[i].transform = *mRenderObjects[ mDirtyObjectIndices[i]].transform;
		objData[i].sphereBounds = *mRenderObjects[ mDirtyObjectIndices[i]].sphereBounds;
		mRenderObjects[i].updated = false;

		copies.PushBack(
			vk::BufferCopy{}
				.setSrcOffset( i * sizeof( ObjectData) )
				.setDstOffset( mDirtyObjectIndices[i] * sizeof( ObjectData) )
				.setSize( sizeof( ObjectData) )
		);
	}
	cmd.copyBuffer(stagingBuffer.buffer, mObjectBuffer.buffer, (U32)copies.GetSize(), copies.Data());

	mDirtyObjectIndices.Clear();

	{
		std::lock_guard<std::mutex> lock( mUpdateMutex);

		mPreCullBarriers.PushBack( 
			vk::BufferMemoryBarrier{}
				.setBuffer( mObjectBuffer.buffer)
				.setSize( VK_WHOLE_SIZE)
				.setSrcAccessMask( vk::AccessFlagBits::eTransferWrite)
				.setDstAccessMask( vk::AccessFlagBits::eShaderRead)
		);

		// cleanup
		CurrentFrame().deletionStack.Push( [&, stagingBuffer]() {

			pCore->Device().destroyBuffer( stagingBuffer.buffer);
			pCore->Device().freeMemory( stagingBuffer.bufferMemory);
		});
	}
}


void Renderer::CreateBuffer( Buffer &buffer, Size size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryType) {

	if ( (bool)buffer.buffer)
	{
		std::lock_guard<std::mutex> lock( mUpdateMutex);

		NextFrame().deletionStack.Push([&, buffer](){

			pCore->Device().destroyBuffer( buffer.buffer);
			pCore->Device().freeMemory( buffer.bufferMemory);
		});
	}

	buffer.bufferSize = size;
	buffer.usage = usage;
	buffer.memoryType = memoryType;

	buffer.buffer = pCore->CreateBuffer( buffer.bufferSize, buffer.usage);
	buffer.bufferMemory = pCore->AllocateBufferMemory( buffer.buffer, buffer.memoryType);
}


Buffer Renderer::CreateStagingBuffer(Size bufferSize) {

	Buffer stagingBuffer;
	stagingBuffer.bufferSize = bufferSize;
	stagingBuffer.usage = vk::BufferUsageFlagBits::eTransferSrc;
	stagingBuffer.memoryType = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	stagingBuffer.buffer = pCore->CreateBuffer( bufferSize, stagingBuffer.usage);
	stagingBuffer.bufferMemory = pCore->AllocateBufferMemory( stagingBuffer.buffer, stagingBuffer.memoryType);

	return stagingBuffer;
}


void Renderer::CreateSubmitContexts() {

	mGraphicsSubmit.queue = pCore->GraphicsQueue();
	mGraphicsSubmit.fence = pCore->CreateFence();
	mGraphicsSubmit.commandPool = pCore->CreateCommandPool( pCore->QueueFamilies().graphicsFamily);
	mGraphicsSubmit.commandBuffer = pCore->AllocateCommandBuffer( mGraphicsSubmit.commandPool);

	mTransferSubmit.queue = pCore->TransferQueue();
	mTransferSubmit.fence = pCore->CreateFence();
	mTransferSubmit.commandPool = pCore->CreateCommandPool( pCore->QueueFamilies().transferFamily);
	mTransferSubmit.commandBuffer = pCore->AllocateCommandBuffer( mTransferSubmit.commandPool);

	mDeletionStack.Push( [&](){
		pCore->Device().destroyFence( mGraphicsSubmit.fence);
		pCore->Device().destroyCommandPool( mGraphicsSubmit.commandPool);

		pCore->Device().destroyFence( mTransferSubmit.fence);
		pCore->Device().destroyCommandPool( mTransferSubmit.commandPool);
	});
}


void Renderer::CreateFrameResources() {

	mFrames.Resize( pFrameOverlap->Get());
	for (auto &frame : mFrames)
	{
		frame.renderFence = pCore->CreateFence( vk::FenceCreateFlagBits::eSignaled);
		frame.renderSemaphore = pCore->CreateSemaphore();
		frame.presentSemaphore = pCore->CreateSemaphore();

		frame.commandPool = pCore->CreateCommandPool( pCore->QueueFamilies().graphicsFamily);
		frame.commandBuffer = pCore->AllocateCommandBuffer( frame.commandPool);

		frame.descriptorAllocator = mMemory.New<DescriptorSetAllocator>();
		frame.descriptorAllocator->Init( pCore->Device());

		mDeletionStack.Push( [&](){
			pCore->Device().destroyFence( frame.renderFence, nullptr);
			pCore->Device().destroySemaphore( frame.renderSemaphore, nullptr);
			pCore->Device().destroySemaphore( frame.presentSemaphore, nullptr);
			
			pCore->Device().destroyCommandPool( frame.commandPool);

			frame.descriptorAllocator->DestroyPools();
			mMemory.Delete( frame.descriptorAllocator);
		});
	}	
}


void Renderer::CreateDefaultPipelineBuilder() {

	mDefaultPipelineBuilder.descriptorLayouts = {};
	mDefaultPipelineBuilder.shaderInfos = {};

	// vertex input
	mDefaultPipelineBuilder.vertexInputBinding = Vertex::getBindingDescription();
	mDefaultPipelineBuilder.vertexAttributes = Vertex::getAttributeDescriptions();
	mDefaultPipelineBuilder.vertexInputInfo
		.setVertexBindingDescriptionCount( 1 )
		.setPVertexBindingDescriptions( &mDefaultPipelineBuilder.vertexInputBinding )
		.setVertexAttributeDescriptionCount( (U32)mDefaultPipelineBuilder.vertexAttributes.GetSize() )
		.setPVertexAttributeDescriptions( mDefaultPipelineBuilder.vertexAttributes.Data() );			
			
	// input assembly
	mDefaultPipelineBuilder.inputAssemblyInfo
		.setTopology( vk::PrimitiveTopology::eTriangleList )
		.setPrimitiveRestartEnable( VK_FALSE );

	mDefaultPipelineBuilder.viewportInfo
		.setViewportCount( 1 )
		.setScissorCount( 1 );
			
	// rasterization
	mDefaultPipelineBuilder.rasterizerInfo
		.setDepthClampEnable( VK_FALSE )
		.setRasterizerDiscardEnable( VK_FALSE )
		.setPolygonMode( vk::PolygonMode::eFill )
		.setLineWidth( 1.0 )
		.setCullMode( vk::CullModeFlagBits::eBack )
		.setFrontFace( vk::FrontFace::eCounterClockwise)
		.setDepthBiasEnable( VK_FALSE )
		.setDepthBiasConstantFactor( 0.0f )
		.setDepthBiasClamp( 0.0f )
		.setDepthBiasSlopeFactor( 0.0f );
		
	// multisampling
	vk::SampleCountFlagBits sampleCount;
	switch ( pMsaaSamples->Get())
	{
		case 64:
			sampleCount = vk::SampleCountFlagBits::e64;
			break;
		case 32:
			sampleCount = vk::SampleCountFlagBits::e32;
			break;
		case 16:
			sampleCount = vk::SampleCountFlagBits::e16;
			break;
		case 8:
			sampleCount = vk::SampleCountFlagBits::e8;
			break;
		case 4:
			sampleCount = vk::SampleCountFlagBits::e4;
			break;
		case 2:
			sampleCount = vk::SampleCountFlagBits::e2;
			break;
		
		default:
			sampleCount = vk::SampleCountFlagBits::e1;
			break;
	}
	mDefaultPipelineBuilder.multisampleInfo
		.setSampleShadingEnable( pMsaaSamples->Get() > 1 )
		.setMinSampleShading( 1.0f )
		.setRasterizationSamples( sampleCount )
		.setPSampleMask( nullptr )
		.setAlphaToCoverageEnable( VK_FALSE )
		.setAlphaToOneEnable( VK_FALSE );
		
	// depth and stencil
	mDefaultPipelineBuilder.depthStencilInfo
		.setDepthTestEnable( VK_TRUE )
		.setDepthWriteEnable( VK_TRUE )
		.setDepthCompareOp( vk::CompareOp::eLess )
		.setDepthBoundsTestEnable( VK_FALSE ) 
		.setMinDepthBounds( 0.0f )
		.setMaxDepthBounds( 1.0f )
		.setStencilTestEnable( VK_FALSE )
		.setFront( {} )
		.setBack( {} );	
				
	// color blending
	mDefaultPipelineBuilder.colorBlendAttachment
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
		
	mDefaultPipelineBuilder.colorBlendInfo
		.setAttachmentCount( 1 )
		.setPAttachments( &mDefaultPipelineBuilder.colorBlendAttachment );

		// dynamic states
	mDefaultPipelineBuilder.dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
	mDefaultPipelineBuilder.dynamicStateInfo = vk::PipelineDynamicStateCreateInfo{}
		.setDynamicStateCount( (U32)mDefaultPipelineBuilder.dynamicStates.GetSize() )
		.setPDynamicStates( mDefaultPipelineBuilder.dynamicStates.Data() );
}


void Renderer::CreateMeshPass( MeshPass *pass, PassType type) {

	pass->passType = type;

	CreateBuffer(
		pass->drawIndirectBuffer,
		sizeof(IndirectData),
		vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eIndirectBuffer | vk::BufferUsageFlagBits::eTransferDst,
		// vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent 
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);
	
	CreateBuffer(
		pass->instanceDataBuffer,
		sizeof(InstanceData),
		vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
		// vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent 
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);
	
	CreateBuffer(
		pass->instanceIndexBuffer,
		sizeof(U32),
		vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);
}


void Renderer::CreateDescriptorResources() {

	// camera data
	CreateBuffer(
		mCameraBuffer, sizeof(CameraData),
		vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);
	mDeletionStack.Push( [&](){
		pCore->Device().destroyBuffer( mCameraBuffer.buffer);
		pCore->Device().freeMemory( mCameraBuffer.bufferMemory);
	});
	
	// scene data
	CreateBuffer(
		mSceneBuffer, sizeof(SceneData),
		vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);
	mDeletionStack.Push( [&](){
		pCore->Device().destroyBuffer( mSceneBuffer.buffer);
		pCore->Device().freeMemory( mSceneBuffer.bufferMemory);
	});

	// object data
	CreateBuffer( 
		mObjectBuffer, sizeof(ObjectData), 
		vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst, 
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);

	// light data
	CreateBuffer(
		mLightBuffer, sizeof( glm::mat4),
		vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst, 
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);
	mDeletionStack.Push( [&](){
		pCore->Device().destroyBuffer( mLightBuffer.buffer);
		pCore->Device().freeMemory( mLightBuffer.bufferMemory);
	});
}


void Renderer::CreateDescriptorSetLayouts() {

	auto cameraDataBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 0 )
		.setDescriptorType( vk::DescriptorType::eUniformBuffer )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment );
	auto sceneDataBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 1 )
		.setDescriptorType( vk::DescriptorType::eUniformBuffer )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment );
	auto shadowImageBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 2 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eFragment );
	
	Array<vk::DescriptorSetLayoutBinding> globalBindings = {
		cameraDataBinding, sceneDataBinding, shadowImageBinding
	};
	mGlobalDataLayout = pCore->CreateDescriptorSetLayout( (U32)globalBindings.GetSize(), globalBindings.Data());	

	mDeletionStack.Push( [&](){
		pCore->Device().destroyDescriptorSetLayout( mGlobalDataLayout);
	});
}


void Renderer::CreateDescriptorSets() {

	auto cameraInfo = mCameraBuffer.DescriptorInfo();
	auto sceneInfo = mSceneBuffer.DescriptorInfo();
	auto shadowCascadeInfo = mMainLight.cascadeViewProjBuffer.DescriptorInfo();
	auto shadowMapInfo = mMainLight.shadowImage.DescriptorInfo( mShadowSampler);
	// auto shadowInfo = mShadowImage.DescriptorInfo( mShadowSampler);
	mGlobalDataSet = DescriptorSetBuilder( pCore->Device(), pDescriptorSetAllocator, pDescriptorLayoutCache)
		.BindBuffer( 0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, &cameraInfo)
		.BindBuffer( 1, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment, &sceneInfo)
		.BindBuffer( 2, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eFragment, &shadowCascadeInfo)
		.BindImage( 3, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, &shadowMapInfo)
		.Build();
}


void Renderer::CreateSamplers() {

	CreateSampler( "Samplers/default_depth.sampler.json");
	mDepthSampler = mSamplers[ SID("Samplers/default_depth.sampler.json")];
	CreateSampler( "Samplers/default_shadow.sampler.json");
	mShadowSampler = mSamplers[ SID("Samplers/default_shadow.sampler.json")];

	CreateSampler( "Samplers/default_smooth.sampler.json");
	CreateSampler( "Samplers/default_blocky.sampler.json");
}


void Renderer::CreatePipelines() {

	// pipeline for depth pyramid creation
	CreatePipeline( "Pipelines/depth_reduce.pipeline.json");
	mDepthReducePipeline = mPipelines[ SID("Pipelines/depth_reduce.pipeline.json")];
	// pipeline for forward pass culling 
	CreatePipeline( "Pipelines/view_culling.pipeline.json");
	mViewCullPipeline = mPipelines[ SID("Pipelines/view_culling.pipeline.json")];
	// pipeline for shadow culling 
	CreatePipeline( "Pipelines/shadow_culling.pipeline.json");
	mShadowCullPipeline = mPipelines[ SID("Pipelines/shadow_culling.pipeline.json")];
	// pipeline to render full screen image
	CreatePipeline( "Pipelines/default_fullscreen_texture.pipeline.json", mForwardPass);
	// pipeline to render to (directional) shadow image
	CreatePipeline( "Pipelines/default_shadow.pipeline.json", mShadowPass);
}


void Renderer::UploadBufferData( void *bufferData, Size size, vk::Buffer targetBuffer, Size offset) {

	Buffer stagingBuffer = CreateStagingBuffer( size);

	// transfer data to staging buffer
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
	else if (initialLayout == vk::ImageLayout::eUndefined && finalLayout == vk::ImageLayout::eGeneral ) 
	{	
		barrier.setSrcAccessMask( vk::AccessFlagBits::eNone );
		barrier.setDstAccessMask( vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite);
		
		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe; 
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
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

	auto context = mTransferSubmit;
	// auto context = mGraphicsSubmit;
	vk::CommandBuffer cmd = context.commandBuffer;
	
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
	result = context.queue.submit( 1, &submitInfo, context.fence);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while submitting to transfer queue " + vk::to_string(result));
	}

	result = pCore->Device().waitForFences( 1, &context.fence, true, 1e9);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while waiting for fence " + vk::to_string(result));
	}
	result = pCore->Device().resetFences( 1, &context.fence);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while resetting fence " + vk::to_string(result));
	}
	pCore->Device().resetCommandPool( context.commandPool);
}


void Renderer::CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, Size offset, Size bufferSize) {

	auto context = mTransferSubmit;	
	// auto context = mGraphicsSubmit;	
	vk::CommandBuffer cmd = context.commandBuffer;

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
	result = context.queue.submit( 1, &submitInfo, context.fence);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while submitting to transfer queue " + vk::to_string(result));
	}

	result = pCore->Device().waitForFences( 1, &context.fence, true, 1e9);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while waiting for fence " + vk::to_string(result));
	}
	result = pCore->Device().resetFences( 1, &context.fence);
	if( result != vk::Result::eSuccess ) 
	{
		throw std::runtime_error("Error while resetting fence " + vk::to_string(result));
	}
	pCore->Device().resetCommandPool( context.commandPool);
}


void Renderer::UpdateCameraData() {

	CameraData camera;
	camera.view = mMainCamera.View();
	camera.proj = mMainCamera.Projection();
	camera.viewProj = camera.proj * camera.view;
	camera.position = glm::vec4( mMainCamera.position, mMainCamera.zoom); 
	
	UploadBufferData( &camera, sizeof(CameraData), mCameraBuffer.buffer);
}


void Renderer::UpdateSceneData() {

	float angle = (float)mFrameCount / 240.f * glm::radians(30.f);
	mMainLight.direction = {cos(angle), -1.f, sin(angle)};

	SceneData scene;
	scene.ambientColor = glm::vec3(1.f, 1.f, 1.f);
	scene.ambientIntensity = 0.03f;
	scene.sunColor = glm::vec3(1.f, 1.f, 1.f);
	scene.sunIntensity = 2.5f;
	scene.sunDirection = mMainLight.direction;

	UploadBufferData( &scene, sizeof( SceneData), mSceneBuffer.buffer);

	// TODO change later for multiple lights -> seperate update lights method
	// auto viewProj = glm::orthoZO( -(float)mShadowExtent.width, (float)mShadowExtent.width, 
	// 							   (float)mShadowExtent.height, -(float)mShadowExtent.height, 
	// 							  -(float)mShadowExtent.depth, (float)mShadowExtent.depth ) *
	// 				glm::lookAt( mMainCamera.position, mMainCamera.position + mMainLight.direction, mMainCamera.up);
	// UploadBufferData( &viewProj, sizeof( glm::mat4), mLightBuffer.buffer);
}


void Renderer::UpdateShadowCascade() {

	float minDepth = mMainCamera.zNear;
	float maxDepth = mMainCamera.zFar;
	float depthRange = maxDepth - minDepth;
	float depthRatio = maxDepth / minDepth;

	float cascadeSplits[ pShadowCascadeCount->Get()];
	// calculate split depths based on view camera frustum
	// based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
	for (U32 i = 0; i < pShadowCascadeCount->Get(); i++) 
	{
		float p = ((float)i + 1.f) / static_cast<float>( pShadowCascadeCount->Get());
		float log = minDepth * std::pow( depthRatio, p);
		float uniform = minDepth + depthRange * p;
		float d = pShadowCascadeSplit->Get() * (log - uniform) + uniform;
		cascadeSplits[i] = (d - minDepth) / depthRange;
	}
	// calculate orthographic projection matrix for each cascade
	glm::mat4 cascadeViews[ pShadowCascadeCount->Get()];
	glm::mat4 cascadeViewProjs[ pShadowCascadeCount->Get()];
	float lastSplitDist = 0.0;
	for (U32 i = 0; i < pShadowCascadeCount->Get(); i++) 
	{
		float splitDist = cascadeSplits[i];

		glm::vec3 frustumCorners[8] = {
			glm::vec3(-1.0f,  1.0f, 0.0f),
			glm::vec3( 1.0f,  1.0f, 0.0f),
			glm::vec3( 1.0f, -1.0f, 0.0f),
			glm::vec3(-1.0f, -1.0f, 0.0f),
			glm::vec3(-1.0f,  1.0f,  1.0f),
			glm::vec3( 1.0f,  1.0f,  1.0f),
			glm::vec3( 1.0f, -1.0f,  1.0f),
			glm::vec3(-1.0f, -1.0f,  1.0f),
		};

		// project frustum corners into world space
		glm::mat4 invCam = glm::inverse( mMainCamera.Projection() * mMainCamera.View());
		for (U32 j = 0; j < 8; j++) 
		{
			glm::vec4 invCorner = invCam * glm::vec4( frustumCorners[j], 1.0f);
			frustumCorners[j] = invCorner / invCorner.w;
		}
		// get current frustum slice
		for (U32 j = 0; j < 4; j++) 
		{
			glm::vec3 dist = frustumCorners[j + 4] - frustumCorners[j];
			frustumCorners[j + 4] = frustumCorners[j] + (dist * splitDist);
			frustumCorners[j] = frustumCorners[j] + (dist * lastSplitDist);
		}

		// calculate frustum slice sphere bounds
		glm::vec3 frustumCenter = glm::vec3(0.0f);
		for (U32 j = 0; j < 8; j++) 
		{
			frustumCenter += frustumCorners[j];
		}
		frustumCenter /= 8.0f;
		float radius = 0.0f;
		for (U32 j = 0; j < 8; j++) 
		{
			float distance = glm::length(frustumCorners[j] - frustumCenter);
			radius = glm::max(radius, distance);
		}
		radius = std::ceil(radius * 16.0f) / 16.0f;

		// calculate view and projection matrix
		glm::vec3 lightDir = mMainLight.direction;
		mMainLight.cascades[i].view = glm::lookAt( frustumCenter - lightDir * 2.f * radius, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
		mMainLight.cascades[i].proj = glm::ortho( -radius, radius, radius, -radius, 0.0f, 4 * radius);

		// Store split distance
		mMainLight.cascades[i].splitDepth = (mMainCamera.zNear + splitDist * depthRange) * -1.0f; // -1 because of camera z direction

		// cascade data to send to GPU
		cascadeViewProjs[i] = mMainLight.cascades[i].proj * mMainLight.cascades[i].view;
		cascadeViewProjs[i][3][3] = mMainLight.cascades[i].splitDepth; // store splitdepth for check for cascade index in forward pass
		cascadeViews[i] = mMainLight.cascades[i].view;
		cascadeViews[i][3][3] = radius; // store radius to use in aabb culling in shadow cull pass

		lastSplitDist = cascadeSplits[i];
	}

	// send view matrix data to GPU
	UploadBufferData(cascadeViews, pShadowCascadeCount->Get() * sizeof( glm::mat4), mMainLight.cascadeViewBuffer.buffer);
	UploadBufferData(cascadeViewProjs, pShadowCascadeCount->Get() * sizeof( glm::mat4), mMainLight.cascadeViewProjBuffer.buffer);
}


void Renderer::DrawFrame() {

	auto &frame = CurrentFrame();

	vk::Result result;

	// get next swapchain image
	U32 imageIndex;
	try
	{
		result = pCore->Device().acquireNextImageKHR(
			mSwapchain.swapchain, U64_MAX, frame.renderSemaphore, nullptr, &imageIndex
		);
	}
	catch ( ... )
	{
		if( result == vk::Result::eErrorOutOfDateKHR)
		{
			std::cout << "out of date swapchain\n";
			RecreateSwapchain();   // TODO maybe handle in glfwResizeCallback instead ?
			return;
		}
		else if( result != vk::Result::eSuccess)
		{
			throw std::runtime_error("Error while getting swapchain image " + vk::to_string(result));
		}
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


	frame.descriptorAllocator->ResetPools();
	frame.deletionStack.Flush();


	// get current frames command buffer and reset
	auto &cmd = frame.commandBuffer;
	cmd.reset( vk::CommandBufferResetFlagBits::eReleaseResources);	


	// record commands
	auto cmdBeginInfo = vk::CommandBufferBeginInfo{}
		.setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );		
	cmd.begin(cmdBeginInfo);


	// buffer updates
	mPreCullBarriers.Clear();


	// UpdateCameraData();
	// UpdateSceneData();
	// UpdateShadowCascade();

	// UpdateObjectBuffer( cmd);

    // UpdateMeshPassBatchBuffer( &mShadowMeshPass, cmd);
    // UpdateMeshPassInstanceBuffer( &mShadowMeshPass, cmd);
    // UpdateMeshPassBatchBuffer( &mOpaqueMeshPass, cmd);
    // UpdateMeshPassInstanceBuffer( &mOpaqueMeshPass, cmd);
    // UpdateMeshPassBatchBuffer( &mTransparentMeshPass, cmd);
    // UpdateMeshPassInstanceBuffer( &mTransparentMeshPass, cmd);


	auto preCullUpdates = mJobs.CreateJob( [](void*){}, nullptr);

	auto sceneUpdate =  mJobs.CreateJob( [&](void*){
		UpdateCameraData();
		UpdateSceneData();
		UpdateShadowCascade();
	}, nullptr, preCullUpdates);
	mJobs.Run( sceneUpdate);

	auto objectUpdate =  mJobs.CreateJob( [&](void*){
		UpdateObjectBuffer( cmd);
	}, nullptr, preCullUpdates);
	mJobs.Run( objectUpdate);

	auto shadowBatchUpdate =  mJobs.CreateJob( [&](void *data){
		UpdateMeshPassBatchBuffer( (MeshPass*)data, cmd);
	}, &mShadowMeshPass, preCullUpdates);
	auto shadowInstanceUpdate =  mJobs.CreateJob( [&](void *data){
		UpdateMeshPassInstanceBuffer( (MeshPass*)data, cmd);
	}, &mShadowMeshPass, preCullUpdates);
	mJobs.Run( shadowBatchUpdate);
	mJobs.Run( shadowInstanceUpdate);

	auto opaqueBatchUpdate =  mJobs.CreateJob( [&](void *data){
		UpdateMeshPassBatchBuffer( (MeshPass*)data, cmd);
	}, &mOpaqueMeshPass, preCullUpdates);
	auto opaqueInstanceUpdate =  mJobs.CreateJob( [&](void *data){
		UpdateMeshPassInstanceBuffer( (MeshPass*)data, cmd);
	}, &mOpaqueMeshPass, preCullUpdates);
	mJobs.Run( opaqueBatchUpdate);
	mJobs.Run( opaqueInstanceUpdate);

	auto transparentBatchUpdate =  mJobs.CreateJob( [&](void *data){
		UpdateMeshPassBatchBuffer( (MeshPass*)data, cmd);
	}, &mTransparentMeshPass, preCullUpdates);
	auto transparentInstanceUpdate =  mJobs.CreateJob( [&](void *data){
		UpdateMeshPassInstanceBuffer( (MeshPass*)data, cmd);
	}, &mTransparentMeshPass, preCullUpdates);
	mJobs.Run( transparentBatchUpdate);
	mJobs.Run( transparentInstanceUpdate);

	mJobs.Run( preCullUpdates);
	mJobs.Wait( preCullUpdates);


	cmd.pipelineBarrier( 
		vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eComputeShader, vk::DependencyFlags{}, 
		0, nullptr, (U32)mPreCullBarriers.GetSize(), mPreCullBarriers.Data(), 0, nullptr
	);


	// compute culling
	mPostCullBarriers.Clear();
	
	CullShadowPass( cmd);
	CullForwardPass( cmd);

	cmd.pipelineBarrier( 
		vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eDrawIndirect, vk::DependencyFlags{}, 
		0, nullptr, (U32)mPostCullBarriers.GetSize(), mPostCullBarriers.Data(), 0, nullptr
	);


	// render passes
	DrawShadowPass( cmd, imageIndex);
	DrawForwardPass( cmd, imageIndex);


	// depth pyramid
	UpdateDepthPyramid( cmd);
	
	
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
	
	try
	{
		result = pCore->GraphicsQueue().presentKHR( presentInfo);
	}
	catch( ... )
	{
		if( result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || 
			mSwapchain.extent != pCore->SurfaceExtent() ) 
		{
			RecreateSwapchain();
			return;
		}
		else if( result != vk::Result::eSuccess ) 
		{
			throw std::runtime_error("Error while presenting image " + vk::to_string(result));
		}
	}

	++mFrameCount;	
}


void Renderer::CullShadowPass( vk::CommandBuffer cmd) {

	if ( mCombinedVertexBuffer.bufferSize == 0)
	{
		return;
	}

	DirectionalCullData shadowCull;
	shadowCull.drawCount = (U32)mShadowMeshPass.renderBatches.GetSize();
	shadowCull.cascadeCount = pShadowCascadeCount->Get();


	// build descriptor sets
	auto objectInfo = mObjectBuffer.DescriptorInfo();
	auto instanceDataShadowInfo = mShadowMeshPass.instanceDataBuffer.DescriptorInfo();
	auto drawIndirectShadowInfo = mShadowMeshPass.drawIndirectBuffer.DescriptorInfo();
	auto instanceIdxShadowInfo  = mShadowMeshPass.instanceIndexBuffer.DescriptorInfo();
	auto cascadeViewInfo        = mMainLight.cascadeViewBuffer.DescriptorInfo();
	vk::DescriptorSet shadowCullSet = DescriptorSetBuilder( pCore->Device(), CurrentFrame().descriptorAllocator, pDescriptorLayoutCache)
		.BindBuffer( 0, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &objectInfo)
		.BindBuffer( 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &instanceDataShadowInfo)
		.BindBuffer( 2, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &drawIndirectShadowInfo)
		.BindBuffer( 3, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &instanceIdxShadowInfo)
		.BindBuffer( 4, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &cascadeViewInfo)
		.Build();

	// dispatch culls
	cmd.bindPipeline( vk::PipelineBindPoint::eCompute, mShadowCullPipeline.pipeline);

	cmd.bindDescriptorSets( vk::PipelineBindPoint::eCompute, mShadowCullPipeline.pipelineLayout, 0, 1, &shadowCullSet, 0, nullptr);
	cmd.pushConstants( mShadowCullPipeline.pipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, sizeof( DirectionalCullData), &shadowCull);

	cmd.dispatch( (U32)(shadowCull.drawCount / 256) + 1, 1, 1);

	// set barriers between buffer writes and indirect draws
	mPostCullBarriers.PushBack(
		vk::BufferMemoryBarrier{}
			.setBuffer( mShadowMeshPass.drawIndirectBuffer.buffer)
			.setSize( VK_WHOLE_SIZE)
			.setSrcAccessMask( vk::AccessFlagBits::eShaderWrite)
			.setDstAccessMask( vk::AccessFlagBits::eIndirectCommandRead)
			.setSrcQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
			.setDstQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
	);
	mPostCullBarriers.PushBack(
		vk::BufferMemoryBarrier{}
			.setBuffer( mShadowMeshPass.instanceIndexBuffer.buffer)
			.setSize( VK_WHOLE_SIZE)
			.setSrcAccessMask( vk::AccessFlagBits::eShaderWrite)
			.setDstAccessMask( vk::AccessFlagBits::eIndirectCommandRead)
			.setSrcQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
			.setDstQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
	);
}
    

void Renderer::CullForwardPass( vk::CommandBuffer cmd) {

	if ( mCombinedVertexBuffer.bufferSize == 0)
	{
		return;
	}


	// get cull data
	glm::mat4 proj = mMainCamera.Projection();
	glm::mat4 projT = glm::transpose( proj);
	// normalls of left and upper frustum plane ( pointing inward )
	glm::vec4 frustumX = (projT[3] + projT[0]) / glm::length( glm::vec3( projT[3] + projT[0]));
	glm::vec4 frustumY = (projT[3] + projT[1]) / glm::length( glm::vec3( projT[3] + projT[1]));

	ViewCullData opaqueCull;
	opaqueCull.view = mMainCamera.View();
	opaqueCull.P00 = proj[0][0];
	opaqueCull.P11 = proj[1][1];
	opaqueCull.zNear = -mMainCamera.zNear;
	opaqueCull.zFar = -mMainCamera.zFar;
	opaqueCull.frustum[0] = frustumX.x;
	opaqueCull.frustum[1] = frustumX.z;
	opaqueCull.frustum[2] = frustumY.y;
	opaqueCull.frustum[3] = frustumY.z;
	opaqueCull.pyramidWidth = (float)mDepthPyramid.width;
	opaqueCull.pyramidHeight = (float)mDepthPyramid.height;
	opaqueCull.drawCount = (U32)mOpaqueMeshPass.renderBatches.GetSize();
	opaqueCull.enableDistCull = 1;
	opaqueCull.enableOcclusionCull = 1;

	ViewCullData transparentCull = opaqueCull;
	transparentCull.drawCount = (U32)mTransparentMeshPass.renderBatches.GetSize(); 

	// build descriptor sets
	auto pyramidInfo = mDepthPyramid.DescriptorInfo( mDepthSampler, vk::ImageLayout::eGeneral);
	auto objectInfo = mObjectBuffer.DescriptorInfo();
	auto instanceDataOpaqueInfo = mOpaqueMeshPass.instanceDataBuffer.DescriptorInfo();
	auto drawIndirectOpaqueInfo = mOpaqueMeshPass.drawIndirectBuffer.DescriptorInfo();
	auto instanceIdxOpaqueInfo = mOpaqueMeshPass.instanceIndexBuffer.DescriptorInfo();
	vk::DescriptorSet viewCullOpaqueSet = DescriptorSetBuilder( pCore->Device(), CurrentFrame().descriptorAllocator, pDescriptorLayoutCache)
		.BindImage( 0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eCompute, &pyramidInfo)
		.BindBuffer( 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &objectInfo)
		.BindBuffer( 2, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &instanceDataOpaqueInfo)
		.BindBuffer( 3, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &drawIndirectOpaqueInfo)
		.BindBuffer( 4, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &instanceIdxOpaqueInfo)
		.Build();

	auto instanceDataTransparentInfo = mTransparentMeshPass.instanceDataBuffer.DescriptorInfo();
	auto drawIndirectTransparentInfo = mTransparentMeshPass.drawIndirectBuffer.DescriptorInfo();
	auto instanceIdxTransparentInfo = mTransparentMeshPass.instanceIndexBuffer.DescriptorInfo();
	vk::DescriptorSet viewCullTransparentSet = DescriptorSetBuilder( pCore->Device(), CurrentFrame().descriptorAllocator, pDescriptorLayoutCache)
		.BindImage( 0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eCompute, &pyramidInfo)
		.BindBuffer( 1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &objectInfo)
		.BindBuffer( 2, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &instanceDataTransparentInfo)
		.BindBuffer( 3, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &drawIndirectTransparentInfo)
		.BindBuffer( 4, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, &instanceIdxTransparentInfo)
		.Build();

	// dispatch culls
	cmd.bindPipeline( vk::PipelineBindPoint::eCompute, mViewCullPipeline.pipeline);

	cmd.bindDescriptorSets( vk::PipelineBindPoint::eCompute, mViewCullPipeline.pipelineLayout, 0, 1, &viewCullOpaqueSet, 0, nullptr);
	cmd.pushConstants( mViewCullPipeline.pipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, sizeof( ViewCullData), &opaqueCull);

	cmd.dispatch( (U32)(opaqueCull.drawCount / 256) + 1, 1, 1);

	cmd.bindDescriptorSets( vk::PipelineBindPoint::eCompute, mViewCullPipeline.pipelineLayout, 0, 1, &viewCullTransparentSet, 0, nullptr);
	cmd.pushConstants( mViewCullPipeline.pipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, sizeof( ViewCullData), &transparentCull);

	cmd.dispatch( (U32)(transparentCull.drawCount / 256) + 1, 1, 1);

	// set barriers between buffer writes and indirect draws
	mPostCullBarriers.PushBack(
		vk::BufferMemoryBarrier{}
			.setBuffer( mOpaqueMeshPass.drawIndirectBuffer.buffer)
			.setSize( VK_WHOLE_SIZE)
			.setSrcAccessMask( vk::AccessFlagBits::eShaderWrite)
			.setDstAccessMask( vk::AccessFlagBits::eIndirectCommandRead)
			.setSrcQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
			.setDstQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
	);
	mPostCullBarriers.PushBack(
		vk::BufferMemoryBarrier{}
			.setBuffer( mOpaqueMeshPass.instanceIndexBuffer.buffer)
			.setSize( VK_WHOLE_SIZE)
			.setSrcAccessMask( vk::AccessFlagBits::eShaderWrite)
			.setDstAccessMask( vk::AccessFlagBits::eIndirectCommandRead)
			.setSrcQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
			.setDstQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
	);
	mPostCullBarriers.PushBack(
		vk::BufferMemoryBarrier{}
			.setBuffer( mTransparentMeshPass.drawIndirectBuffer.buffer)
			.setSize( VK_WHOLE_SIZE)
			.setSrcAccessMask( vk::AccessFlagBits::eShaderWrite)
			.setDstAccessMask( vk::AccessFlagBits::eIndirectCommandRead)
			.setSrcQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
			.setDstQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
	);
	mPostCullBarriers.PushBack(
		vk::BufferMemoryBarrier{}
			.setBuffer( mTransparentMeshPass.instanceIndexBuffer.buffer)
			.setSize( VK_WHOLE_SIZE)
			.setSrcAccessMask( vk::AccessFlagBits::eShaderWrite)
			.setDstAccessMask( vk::AccessFlagBits::eIndirectCommandRead)
			.setSrcQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
			.setDstQueueFamilyIndex( pCore->QueueFamilies().graphicsFamily)
	);
}


void Renderer::DrawShadowPass( vk::CommandBuffer cmd, U32 imageIndex) {

	auto clearValue = vk::ClearValue{}
		.setDepthStencil( {1.0f, 0} );

	auto viewport = vk::Viewport{}
		.setX( 0.f ).setY( 0.f )
		.setWidth( (float)mMainLight.shadowImage.width ).setHeight( (float)mMainLight.shadowImage.height )
		.setMinDepth( 0.f ).setMaxDepth( 1.f );
	auto scissor = vk::Rect2D{}
		.setOffset( {0, 0} )
		.setExtent( {mShadowImage.width, mShadowImage.height} );

	auto renderInfo = vk::RenderPassBeginInfo{}
		.setRenderPass( mShadowPass )
		.setRenderArea( {{0, 0}, {mMainLight.shadowImage.width, mMainLight.shadowImage.height}} )
		.setClearValueCount( 1 )
		.setPClearValues( &clearValue );		
	
	for (U32 i = 0; i< pShadowCascadeCount->Get(); i++)
	{
		renderInfo.setFramebuffer(  mMainLight.cascades[i].framebuffer);
		cmd.beginRenderPass(renderInfo, vk::SubpassContents::eInline);

		// set viewport
		cmd.setViewport( 0, 1, &viewport);
		cmd.setScissor( 0, 1, &scissor);

		if ( mCombinedVertexBuffer.bufferSize > 0)
		{
			Size offset = 0;
			cmd.bindVertexBuffers( 0, 1, &mCombinedVertexBuffer.buffer, &offset);
			cmd.bindIndexBuffer( mCombinedIndexBuffer.buffer, 0, vk::IndexType::eUint32);

			auto lightInfo = mMainLight.cascadeViewProjBuffer.DescriptorInfo();
			auto globalDataSet = DescriptorSetBuilder( pCore->Device(), CurrentFrame().descriptorAllocator, pDescriptorLayoutCache)
				.BindBuffer( 0, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eVertex, &lightInfo)
				.Build();

			RenderMeshPass( cmd, &mShadowMeshPass, globalDataSet,   sizeof( U32), &i, vk::ShaderStageFlagBits::eVertex);
		}

		cmd.endRenderPass();
	}

	// //start renderpass
	// vk::ClearValue clearValue;
	// clearValue.setDepthStencil( {1.0f, 0} ); 

	// auto renderInfo = vk::RenderPassBeginInfo{}
	// 	.setRenderPass( mShadowPass )
	// 	.setRenderArea( {{0, 0}, {mShadowImage.width, mShadowImage.height}} )
	// 	.setFramebuffer( mShadowFramebuffers[imageIndex] )
	// 	.setClearValueCount( 1 )
	// 	.setPClearValues( &clearValue );		
	// cmd.beginRenderPass(renderInfo, vk::SubpassContents::eInline);
	
	// // set viewport
	// auto viewport = vk::Viewport{}
	// 	.setX( 0.f )
	// 	.setY( 0.f )
	// 	.setWidth( (float)mShadowImage.width )
	// 	.setHeight( (float)mShadowImage.height )
	// 	.setMinDepth( 0.f )
	// 	.setMaxDepth( 1.f );
	// auto scissor = vk::Rect2D{}
	// 	.setOffset( {0, 0} )
	// 	.setExtent( {mShadowImage.width, mShadowImage.height} );

	// cmd.setViewport( 0, 1, &viewport);
	// cmd.setScissor( 0, 1, &scissor);


	// if ( mCombinedVertexBuffer.bufferSize > 0)
	// {
	// 	Size offset = 0;
	// 	cmd.bindVertexBuffers( 0, 1, &mCombinedVertexBuffer.buffer, &offset);
	// 	cmd.bindIndexBuffer( mCombinedIndexBuffer.buffer, 0, vk::IndexType::eUint32);

	// 	auto lightInfo = mLightBuffer.DescriptorInfo();
	// 	auto globalDataSet = DescriptorSetBuilder( pCore->Device(), CurrentFrame().descriptorAllocator, pDescriptorLayoutCache)
	// 		.BindBuffer( 0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex, &lightInfo)
	// 		.Build();

	// 	RenderMeshPass( cmd, &mShadowMeshPass, globalDataSet);
	// }


	// cmd.endRenderPass();
}


void Renderer::DrawForwardPass( vk::CommandBuffer cmd, U32 imageIndex) {

	//start renderpass
	vk::ClearValue clearValues[2];
	clearValues[0].setColor( vk::ClearColorValue().setFloat32({0.1f, 0.1f, 0.1f, 1.0f}) );
	clearValues[1].setDepthStencil( {1.0f, 0} ); 

	auto renderInfo = vk::RenderPassBeginInfo{}
		.setRenderPass( mForwardPass )
		.setRenderArea( {{0, 0}, mSwapchain.extent} )
		.setFramebuffer( mForwardFramebuffers[imageIndex] )
		.setClearValueCount( 2 )
		.setPClearValues( clearValues );		
	cmd.beginRenderPass(renderInfo, vk::SubpassContents::eInline);
	
	// set viewport
	auto viewport = vk::Viewport{}
		.setX( 0.f )
		.setY( 0.f )
		.setWidth( (float)mSwapchain.extent.width )
		.setHeight( (float)mSwapchain.extent.height )
		.setMinDepth( 0.f )
		.setMaxDepth( 1.f );
	auto scissor = vk::Rect2D{}
		.setOffset( {0, 0} )
		.setExtent( mSwapchain.extent );

	cmd.setViewport( 0, 1, &viewport);
	cmd.setScissor( 0, 1, &scissor);


	if ( mCombinedVertexBuffer.bufferSize > 0)
	{
		Size offset = 0;
		cmd.bindVertexBuffers( 0, 1, &mCombinedVertexBuffer.buffer, &offset);
		cmd.bindIndexBuffer( mCombinedIndexBuffer.buffer, 0, vk::IndexType::eUint32);

		U32 cascadeCount = pShadowCascadeCount->Get();
		RenderMeshPass( cmd, &mOpaqueMeshPass, mGlobalDataSet,   sizeof( U32), &cascadeCount, vk::ShaderStageFlagBits::eFragment);
		RenderMeshPass( cmd, &mTransparentMeshPass, mGlobalDataSet,   sizeof( U32), &cascadeCount, vk::ShaderStageFlagBits::eFragment);
	}


	cmd.endRenderPass();
}


void Renderer::RenderMeshPass( vk::CommandBuffer cmd, MeshPass *pass, vk::DescriptorSet globalDataSet, U32 pushConstantSize, void *pushConstantData, vk::ShaderStageFlagBits pushConstantStage) {

	auto objectInfo = mObjectBuffer.DescriptorInfo();
	auto instanceInfo = pass->instanceIndexBuffer.DescriptorInfo();
	auto passDataSet = DescriptorSetBuilder( pCore->Device(), CurrentFrame().descriptorAllocator, pDescriptorLayoutCache)
		.BindBuffer(0, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eVertex, &objectInfo)
		.BindBuffer(1, vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eVertex, &instanceInfo)
		.Build();

	U64 lastPipelineId = 0;
	U64 lastMaterialId = 0;
	for (auto &multibatch : pass->multiBatches)
	{
		IndirectBatch batch = pass->indirectBatches[ multibatch.first];

		RenderObject &object = mRenderObjects[ batch.objectIdx];
		Material &material = mMaterials[ object.materialId];
		Pipeline &pipeline = mPipelines[ material.pipelineId[ pass->passType]];

		if ( material.pipelineId[ pass->passType] != lastPipelineId)
		{
			cmd.bindPipeline(
				vk::PipelineBindPoint::eGraphics, pipeline.pipeline
			);

			// TODO rebind should not be necessary, because same set layout ???
			cmd.bindDescriptorSets( 
				vk::PipelineBindPoint::eGraphics, pipeline.pipelineLayout, 0, 1, &globalDataSet, 0, nullptr
			);
			cmd.bindDescriptorSets( 
				vk::PipelineBindPoint::eGraphics, pipeline.pipelineLayout, 1, 1, &passDataSet, 0, nullptr		
			);

			if ( pushConstantSize > 0)
			{
				cmd.pushConstants( pipeline.pipelineLayout, pushConstantStage, 0, pushConstantSize, pushConstantData);
			}

			lastPipelineId = material.pipelineId[ pass->passType];
		}

		if ( object.materialId != lastMaterialId)
		{
			if ( material.descriptorSet[ pass->passType] != vk::DescriptorSet{})
			{
				cmd.bindDescriptorSets( 
					vk::PipelineBindPoint::eGraphics, pipeline.pipelineLayout, 2, 1, &material.descriptorSet[ pass->passType], 0, nullptr		
				);
			}

			lastMaterialId = object.materialId;
		}
		
		cmd.drawIndexedIndirect( pass->drawIndirectBuffer.buffer, multibatch.first * sizeof(IndirectData), multibatch.count, sizeof(IndirectData));
	}
}


void Renderer::UpdateDepthPyramid( vk::CommandBuffer cmd) {

	// place barrier to make sure depth image write has been completed in render pass and change layout to shader read optimal
	vk::ImageMemoryBarrier depthReadBarrier = vk::ImageMemoryBarrier{}
		.setImage( mDepthImage.image )
		.setSrcAccessMask( vk::AccessFlagBits::eDepthStencilAttachmentWrite )
		.setDstAccessMask( vk::AccessFlagBits::eShaderRead )
		.setOldLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal )
		.setNewLayout( vk::ImageLayout::eShaderReadOnlyOptimal )
		.setSubresourceRange( vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1} );
	cmd.pipelineBarrier( vk::PipelineStageFlagBits::eLateFragmentTests, vk::PipelineStageFlagBits::eComputeShader, vk::DependencyFlagBits::eByRegion, 0, nullptr, 0, nullptr, 1, &depthReadBarrier);

	cmd.bindPipeline( vk::PipelineBindPoint::eCompute, mDepthReducePipeline.pipeline);

	U32 depthPyramidLevels = (U32)mDepthPyramidViews.GetSize();
	for ( U32 i = 0; i < depthPyramidLevels; ++i)
	{
		// set image descriptors
		auto srcImageInfo = vk::DescriptorImageInfo{}
			.setSampler( mDepthSampler )
			.setImageView(   i == 0 ? mDepthImage.imageView : mDepthPyramidViews[i-1] )
			.setImageLayout( i == 0 ? vk::ImageLayout::eShaderReadOnlyOptimal : vk::ImageLayout::eGeneral);

		auto dstImageInfo = vk::DescriptorImageInfo{}
			.setSampler( mDepthSampler )
			.setImageView( mDepthPyramidViews[i] )
			.setImageLayout( vk::ImageLayout::eGeneral );

		vk::DescriptorSet depthSet = DescriptorSetBuilder( pCore->Device(), CurrentFrame().descriptorAllocator, pDescriptorLayoutCache)
			.BindImage( 0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eCompute, &srcImageInfo)
			.BindImage( 1, vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eCompute, &dstImageInfo)
			.Build();
		cmd.bindDescriptorSets( vk::PipelineBindPoint::eCompute, mDepthReducePipeline.pipelineLayout, 0, 1, &depthSet, 0, nullptr);

		// set push constants
		uint32_t levelWidth = std::max(mDepthPyramid.width >> i, 1U);
		uint32_t levelHeight = std::max(mDepthPyramid.height >> i, 1U);
		glm::vec2 imageSize = { levelWidth, levelHeight};
		cmd.pushConstants( mDepthReducePipeline.pipelineLayout, vk::ShaderStageFlagBits::eCompute, 0, sizeof( imageSize), &imageSize);

		// dispatch
		cmd.dispatch( (levelWidth + 32 - 1) / 32, (levelHeight + 32 - 1) / 32, 1);
		
		// place barrier between passes
		vk::ImageMemoryBarrier  depthReduceBarrier = vk::ImageMemoryBarrier{}
			.setImage( mDepthPyramid.image )
			.setSrcAccessMask( vk::AccessFlagBits::eShaderWrite )
			.setDstAccessMask( vk::AccessFlagBits::eShaderRead )
			.setOldLayout( vk::ImageLayout::eGeneral )
			.setNewLayout( vk::ImageLayout::eGeneral )
			.setSubresourceRange( vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1} );
		cmd.pipelineBarrier( vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eComputeShader, vk::DependencyFlagBits::eByRegion, 0, nullptr, 0, nullptr, 1, &depthReduceBarrier);
	}
	
	// place one more barrier to make sure depth pyramid is finished before usage and change layout back to depth stencil optimal
	vk::ImageMemoryBarrier  depthWriteBarrier = vk::ImageMemoryBarrier{}
		.setImage( mDepthImage.image )
		.setSrcAccessMask( vk::AccessFlagBits::eShaderRead )
		.setDstAccessMask( vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite )
		.setOldLayout( vk::ImageLayout::eShaderReadOnlyOptimal )
		.setNewLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal )
		.setSubresourceRange( vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1} );
	cmd.pipelineBarrier( vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::DependencyFlagBits::eByRegion, 0, nullptr, 0, nullptr, 1, &depthWriteBarrier);
}

    
} // Atuin
