
#include "Renderer.h"
#include "RendererCore.h"
#include "ResourceManager.h"
#include "SamplerSettings.h"
#include "App.h"
#include "Core/Util/Types.h"
#include "Core/Config/ConfigManager.h"
#include "Core/DataStructures/Json.h"

#include "GLFW/glfw3.h"

#include "stb/stb_image.h"
#include "tinyobjloader/tiny_obj_loader.h"


namespace Atuin {


CVar<U32>* Renderer::pFrameOverlap = ConfigManager::RegisterCVar("Renderer", "FRAME_OVERLAP", (U32)2);
CVar<U32>* Renderer::pMaxAnisotropy = ConfigManager::RegisterCVar("Renderer", "MAX_ANISOTROPY", (U32)8);
CVar<U32>* Renderer::pMsaaSamples = ConfigManager::RegisterCVar("Renderer", "MSAA_SAMPLES", (U32)1);


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
	mFrameCount {0}
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
    
	CreateDepthResources();
	CreateRenderPasses();	
	CreateFramebuffers();

	CreateSubmitContexts();

	CreateVertexBuffer();
	CreateIndexBuffer();

	CreateFrameResources();
	CreateShaderModules();
	CreateSamplers();
	CreateDescriptorResources();

	CreateDescriptorSetLayouts();
	CreateDescriptorPool();
	CreateDescriptorSets();

	CreatePipeline();

	// dummy model load
	LoadModel( mResourceDir + "Meshes/Default/torus.obj");
	UploadBufferData(mVertices.Data(), mVertices.GetSize() * sizeof(mVertices[0]), mCombinedVertexBuffer.buffer);
	UploadBufferData(mIndices.Data(), mIndices.GetSize() * sizeof(mIndices[0]), mCombinedIndexBuffer.buffer);
}


void Renderer::ShutDown() {

	pCore->Device().waitIdle();

	mDeletionStack.Flush();

	// vertex and index buffers
	pCore->Device().destroyBuffer( mCombinedVertexBuffer.buffer);
	pCore->Device().freeMemory( mCombinedVertexBuffer.bufferMemory);
	pCore->Device().destroyBuffer( mCombinedIndexBuffer.buffer);
	pCore->Device().freeMemory( mCombinedIndexBuffer.bufferMemory);

	// presentation related
	DestroyFramebuffers();	

	pCore->Device().destroyImageView( mDepthImage.imageView);
	pCore->Device().destroyImage( mDepthImage.image);
	pCore->Device().freeMemory( mDepthImage.imageMemory);

	for (auto imageView : mSwapchain.imageViews)
	{
		pCore->Device().destroyImageView( imageView);
	}
	pCore->Device().destroySwapchainKHR( mSwapchain.swapchain, nullptr);

	// descriptor 
	mMemory.Delete(( pDescriptorLayoutCache));
	mMemory.Delete( pDescriptorSetAllocator);

	// core
	mMemory.Delete(pCore);
}
    

void Renderer::Update() {

	// TODO make async calls

	UpdateObjectBuffer();
	UpdateMeshPass( &mShadowMeshPass);
	UpdateMeshPass( &mOpaqueMeshPass);
	UpdateMeshPass( &mTransparentMeshPass);

	DrawFrame();
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


void Renderer::CreateRenderPasses() {

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

	// TODO resolve attachment if MSAA is enabled ( MsaaSamples > 1)

	// forward pass
	Array<vk::AttachmentDescription> attachments = {
		presentAttachment, depthAttachment
	};

	mForwardPass = pCore->CreateRenderPass( attachments, 1);

	mDeletionStack.Push( [&](){
		pCore->Device().destroyRenderPass( mForwardPass);
	});

	// shadow pass
}


void Renderer::CreateFramebuffers() {

	mForwardFramebuffers.Resize( mSwapchain.imageCount);
	for (U32 i = 0; i < mSwapchain.imageCount; i++)
	{
		// forward pass
		Array<vk::ImageView> forwardAttachments =  {
			mSwapchain.imageViews[i], mDepthImage.imageView
		};
		mForwardFramebuffers[i] = pCore->CreateFramebuffer( mForwardPass, forwardAttachments, mSwapchain.extent.width, mSwapchain.extent.height);

		// shadow pass

	}
}


void Renderer::DestroyFramebuffers() {

	for (auto &framebuffer : mForwardFramebuffers)
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
	pCore->Device().destroyImageView(mDepthImage.imageView, nullptr);
	pCore->Device().destroyImage(mDepthImage.image, nullptr);
	pCore->Device().freeMemory(mDepthImage.imageMemory, nullptr);	

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


void Renderer::RegisterMeshObject( const MeshObject &object) {

	RenderObject newObject;
	newObject.transform = object.transform;
	newObject.sphereBounds = object.sphereBounds;
	newObject.meshId = RegisterMesh( object.meshName);
	newObject.materialId = RegisterMaterial( object.materialName);
	// newObject.passIndex created after batch processing

	U32 objIdx = (U32)mRenderObjects.GetSize();
	mRenderObjects.PushBack( newObject);
	mDirtyObjectIndices.PushBack( objIdx);
	
	// submit to mesh passes
	newObject.passIndex.Clear( -1);

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


U64 Renderer::RegisterMesh( std::string_view meshName) {

	U64 meshId = SID( meshName.data());
	if ( mMeshes.Find( meshId) == mMeshes.End())
	{
		CreateMesh( meshName);
		// TODO catch fail state if mesh file does not exist -> log warning and return default/error mesh ID
	}

	return meshId;
}


U64 Renderer::RegisterMaterial( std::string_view materialName) {

	U64 materialId = SID( materialName.data());
	if ( mMeshes.Find( materialId) == mMeshes.End())
	{
		CreateMaterial( materialName);
		// TODO catch fail state if material file does not exist -> log warning and return default/error material ID
	}

	return materialId;
}


void Renderer::CreateMesh( std::string_view meshName) {

	U64 meshId = SID( meshName.data());
	Mesh newMesh;

	newMesh.meshData = pResources->GetMesh( meshName);
	newMesh.vertexCount = (U32)newMesh.meshData->vertices.GetSize();
	newMesh.indexCount = (U32)newMesh.meshData->indices.GetSize();

	newMesh.meshName = meshName;
	mMeshes[ meshId] = newMesh;
}


void Renderer::CreateMaterial( std::string_view materialName) {

	U64 materialId = SID( materialName.data());
	Material newMaterial;

	// read material json
	auto content = mFiles.Read( mResourceDir + materialName.data());
	Json materialJson = Json::Load( std::string_view( content.Data(), content.GetSize()));

	// get pipeline
	std::string pipelineName = materialJson.At( "pipeline").ToString();
	U64 pipelineId = SID( pipelineName.c_str());
	if ( mPipelines.Find( pipelineId) == mPipelines.End())
	{
		CreatePipeline( pipelineName);
	}

	// get images and build descriptor sets
	auto textures = materialJson.At( "textures").GetList();
	DescriptorSetBuilder builder( pCore->Device(), pDescriptorSetAllocator, pDescriptorLayoutCache);
	U32 binding = 0;
	for ( auto texture : textures) 
	{
		//texture format
		vk::Format format;
		if ( texture.At( "format").ToString() == "unorm")
		{
			format = vk::Format::eR8G8B8A8Unorm;
		}
		else
		{
			format = vk::Format::eR8G8B8A8Srgb;
		}

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

		auto imageInfo = mTextures[ imageId].DescriptorInfo( mSamplers[ samplerId]);
		builder.BindImage(binding, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, &imageInfo);
		++binding;
	}
	vk::DescriptorSet descriptorSet = builder.Build();

	// set pass data
	newMaterial.usedInPass.Clear( false);
	if ( materialJson.At( "shadow").ToBool())
	{
		newMaterial.usedInPass[PassType::SHADOW] = true;
		newMaterial.pipelineId[PassType::SHADOW] = SID( "Pipelines/default_shadow.pipeline.json");
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
	ImageData *imageData = pResources->GetImage( textureName);

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

	mDeletionStack.Push( [&](){
		pCore->Device().destroyImageView( image.imageView);
		pCore->Device().destroyImage( image.image);
		pCore->Device().freeMemory( image.imageMemory);
	});
}


void Renderer::CreateSampler( std::string_view samplerName) {

	U64 samplerId = SID( samplerName.data());

	// read sampler json
	auto content = mFiles.Read( mResourceDir + samplerName.data());
	Json samplerJson = Json::Load( std::string_view( content.Data(), content.GetSize()));

	vk::Sampler newSampler = pCore->CreateSampler(
		Sampler::GetFilter( samplerJson.At( "minFilter").ToString()), 
		Sampler::GetFilter( samplerJson.At( "magFilter").ToString()), 
		Sampler::GetAddressMode( samplerJson.At( "addressModeU").ToString()), 
		Sampler::GetAddressMode( samplerJson.At( "addressModeV").ToString()), 
		Sampler::GetAddressMode( samplerJson.At( "addressModeW").ToString()),
		samplerJson.At( "enableAnisotropy").ToBool(),
		(float)pMaxAnisotropy->Get(),
		Sampler::GetMipMapMode( samplerJson.At( "mipmapMode").ToString()), 
		(float)samplerJson.At( "minLod").ToFloat(),
		(float)samplerJson.At( "maxLod").ToFloat(),
		(float)samplerJson.At( "minLodBias").ToFloat(),
		samplerJson.At( "enableCompare").ToBool(),
		Sampler::GetCompareOp( samplerJson.At( "compareOp").ToString()),
		Sampler::GetBorder( samplerJson.At( "borderColor").ToString())
	);

	mSamplers[ samplerId] = newSampler;	
}


void Renderer::CreatePipeline( std::string_view pipelineName) {

	U64 pipelineId = SID( pipelineName.data());

	// read pipeline json
	auto content = mFiles.Read( mResourceDir + pipelineName.data());
	Json pipelineJson = Json::Load( std::string_view( content.Data(), content.GetSize()));

	Pipeline newPipeline;
	std::string type = pipelineJson.At( "type").ToString();
	if ( type == "graphics")
	{
		// CreateGraphicsPipeline( newPipeline, pipelineJson); 
	}
	else if ( type == "compute")
	{
		// CreateComputePipeline( newPipeline, pipelineJson);
	}
	else 
	{
		mLog.Warning( LogChannel::GRAPHICS, FormatStr("Unknown pipeline type \"%s\" in \"%s\".", type, pipelineName));
	}

	mPipelines[ pipelineId] = newPipeline;
}


void Renderer::CreateShaderModule( std::string_view shaderName) {

	U64 shaderId = SID( shaderName.data());

	auto vertShaderCode = mFiles.Read( mResourceDir + shaderName.data() , std::ios::binary);
	auto newShader = pCore->CreateShaderModule( vertShaderCode.GetSize(), vertShaderCode.Data() );

	mShaders[ shaderId] = newShader;

	mDeletionStack.Push( [&](){
		pCore->Device().destroyShaderModule( newShader);
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
	if( totalVertices > mCombinedVertexBuffer.bufferSize)
	{
		CreateBuffer( 
			mCombinedVertexBuffer,
			totalVertices * sizeof(Vertex), 
			vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
	}
	if( totalIndices > mCombinedIndexBuffer.bufferSize)
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
		mLog.Info( LogChannel::GRAPHICS, FormatStr( "Deleting %d objects from mesh pass %d.", pass->deleteObjectIndices.GetSize(), (U8)pass->passType));

		// render batches of deleted objects
		Array<RenderBatch> deleteBatches;
		deleteBatches.Reserve( pass->deleteObjectIndices.GetSize());
		for ( U32 idx : pass->deleteObjectIndices)
		{
			pass->reuseObjectIndices.PushBack( idx);
			RenderObject &obj = mRenderObjects[ pass->objectIndices[idx]];

			RenderBatch newBatch;
			newBatch.objectIdx = pass->objectIndices[idx];
			newBatch.sortKey = (U64)obj.materialId << 32 | obj.meshId;

			deleteBatches.PushBack( newBatch);
		}
		pass->deleteObjectIndices.Clear();

		// remove deleted batches from renderBatches array
		std::sort( deleteBatches.Begin(), deleteBatches.End());

		Array<RenderBatch> newBatches;
		newBatches.Reserve( pass->renderBatches.GetSize());
		std::set_difference( pass->renderBatches.Begin(), pass->renderBatches.End(), deleteBatches.Begin(), deleteBatches.End(), newBatches.Begin());
		pass->renderBatches = std::move( newBatches);

		pass->rebuildBatches = true;
		pass->rebuildInstances = true;
	}

	// handle unbatched objects
	if ( !pass->unbatchedIndices.IsEmpty())
	{
		mLog.Info( LogChannel::GRAPHICS, FormatStr( "Adding %d objects to mesh pass %d.", pass->unbatchedIndices.GetSize(), (U8)pass->passType));

		Array<RenderBatch> newBatches;
		newBatches.Reserve( pass->unbatchedIndices.GetSize());
		for ( U32 idx : pass->unbatchedIndices)
		{
			// insert new render object and create renderbatch for it
			U32 newIndex;
			if ( !pass->reuseObjectIndices.IsEmpty())
			{
				newIndex = pass->reuseObjectIndices.Back();
				pass->reuseObjectIndices.PopBack();
				pass->objectIndices[ newIndex] = idx;
			}
			else
			{
				newIndex = (U32)pass->objectIndices.GetSize();
				pass->objectIndices.PushBack( idx);
			}

			RenderObject &obj = mRenderObjects[ pass->objectIndices[idx]];
			obj.passIndex[ pass->passType] = newIndex;
			
			RenderBatch newBatch;
			newBatch.objectIdx = newIndex;
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

		pass->rebuildBatches = true;
		pass->rebuildInstances = true;
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
	if ( pass->instanceIdxBuffer.bufferSize < pass->renderBatches.GetSize() * sizeof(U32))
	{
		CreateBuffer(
			pass->instanceIdxBuffer,
			pass->renderBatches.GetSize() * sizeof(U32),
			vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
	}

	// update gpu side buffers
    UpdateMeshPassBatchBuffer( pass);
    UpdateMeshPassInstanceBuffer( pass);

	pass->rebuildBatches = false;
	pass->rebuildInstances = false;
}


void Renderer::BuildMeshPassBatches( MeshPass *pass) {

	// do nothing if batches are unchanged
	if( !pass->rebuildBatches)
	{
		return;
	}

	// rebuild indirect batches
	pass->indirectBatches.Clear();
	pass->multiBatches.Clear();

	IndirectBatch newBatch;
	newBatch.first = 0;
	newBatch.count = 0;
	newBatch.objectIdx = 0;
	
	MultiBatch newMultiBatch;
	newMultiBatch.first = 0;
	newMultiBatch.count = 1;

	pass->indirectBatches.PushBack( newBatch);
	pass->multiBatches.PushBack( newMultiBatch);

	U64 lastMaterial = mRenderObjects[ pass->renderBatches[0].objectIdx].materialId;
	U64 lastMesh = mRenderObjects[ pass->renderBatches[0].objectIdx].meshId;

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

			if ( obj.materialId != lastMaterial)
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


void Renderer::UpdateMeshPassBatchBuffer( MeshPass *pass) {

	if ( !pass->rebuildBatches)
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
		indirectData[i].objectIdx = 0;
		indirectData[i].drawIndirectCmd
			.setVertexOffset( mesh.firstVertex )
			.setFirstIndex( mesh.firstIndex )
			.setIndexCount( mesh.indexCount )
			.setFirstInstance( batch.first )
			.setInstanceCount( 0 );
	}

	pCore->Device().unmapMemory( stagingBuffer.bufferMemory);

	// upload buffer data to Gpu memory
	CopyBuffer( stagingBuffer.buffer, pass->drawIndirectBuffer.buffer, 0, stagingBuffer.bufferSize);

	// cleanup
	pCore->Device().destroyBuffer( stagingBuffer.buffer);
	pCore->Device().freeMemory( stagingBuffer.bufferMemory);
}
    
	
void Renderer::UpdateMeshPassInstanceBuffer( MeshPass *pass) {

	if ( !pass->rebuildInstances)
	{
		return;
	}

	Size copySize = pass->indirectBatches.GetSize() * sizeof(InstanceData);

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

	// upload buffer data to Gpu memory
	CopyBuffer( stagingBuffer.buffer, pass->drawIndirectBuffer.buffer, 0, stagingBuffer.bufferSize);

	// cleanup
	pCore->Device().destroyBuffer( stagingBuffer.buffer);
	pCore->Device().freeMemory( stagingBuffer.bufferMemory);
}


void Renderer::UpdateObjectBuffer() {

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
			mObjectBuffer,
			copySize,
			vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
			vk::MemoryPropertyFlagBits::eDeviceLocal	
		);
	}

	// TODO allow for partial update if not too many objects changed

	// fill stating buffer
	Buffer stagingBuffer = CreateStagingBuffer( copySize);
	void *data;
	vk::Result result = pCore->Device().mapMemory( stagingBuffer.bufferMemory, 0, copySize, vk::MemoryMapFlags(), &data);
	if( result != vk::Result::eSuccess )
	{
		throw std::runtime_error("Failed to map staging buffer memory " + vk::to_string(result));
	}

	ObjectData *objData = (ObjectData*)data;
	for ( Size i = 0; i < mRenderObjects.GetSize(); i++)
	{
		objData[i].transform = mRenderObjects[i].transform;
	}

	pCore->Device().unmapMemory( stagingBuffer.bufferMemory);

	// upload buffer data to Gpu memory
	CopyBuffer( stagingBuffer.buffer, mObjectBuffer.buffer, 0, stagingBuffer.bufferSize);

	// cleanup
	pCore->Device().destroyBuffer( stagingBuffer.buffer);
	pCore->Device().freeMemory( stagingBuffer.bufferMemory);
}


void Renderer::CreateBuffer( Buffer &buffer, Size size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryType) {

	if ( (bool)buffer.buffer)
	{
		// CurrentFrame().deletionStack.Push([&](){

			pCore->Device().destroyBuffer( buffer.buffer);
			pCore->Device().freeMemory( buffer.bufferMemory);
		// });
	}

	buffer.bufferSize = size;
	buffer.usage = usage;
	buffer.memoryType = memoryType;

	buffer.buffer = pCore->CreateBuffer( buffer.bufferSize, buffer.usage);
	buffer.bufferMemory = pCore->AllocateBufferMemory( buffer.buffer, buffer.memoryType);
}


void Renderer::CreateVertexBuffer() {

	mCombinedVertexBuffer.bufferSize = 10000 * sizeof(Vertex);
	mCombinedVertexBuffer.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
	mCombinedVertexBuffer.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mCombinedVertexBuffer.buffer = pCore->CreateBuffer( mCombinedVertexBuffer.bufferSize, mCombinedVertexBuffer.usage);
	mCombinedVertexBuffer.bufferMemory = pCore->AllocateBufferMemory( mCombinedVertexBuffer.buffer, mCombinedVertexBuffer.memoryType);
}


void Renderer::CreateIndexBuffer() {

	mCombinedIndexBuffer.bufferSize = 30000 * 4;
	mCombinedIndexBuffer.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
	mCombinedIndexBuffer.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mCombinedIndexBuffer.buffer = pCore->CreateBuffer( mCombinedIndexBuffer.bufferSize, mCombinedIndexBuffer.usage);
	mCombinedIndexBuffer.bufferMemory = pCore->AllocateBufferMemory( mCombinedIndexBuffer.buffer, mCombinedIndexBuffer.memoryType);
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
		
	std::unordered_map<Vertex, uint32_t> uniqueVertices;
		
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
				
			if(uniqueVertices.find(vertex) == uniqueVertices.end()) 
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(mVertices.GetSize());
				mVertices.PushBack(vertex);
			}
				
			mIndices.PushBack( uniqueVertices[vertex]);
		}
	}


	// calculate tangents
	for (Size i = 0 ; i < mIndices.GetSize(); i += 3) 
	{
		Vertex& v0 = mVertices[ mIndices[i]];
		Vertex& v1 = mVertices[ mIndices[i+1]];
		Vertex& v2 = mVertices[ mIndices[i+2]];

		glm::vec3 edge1 = v1.position - v0.position;
		glm::vec3 edge2 = v2.position - v0.position;

		float deltaU1 = v1.texCoord.x - v0.texCoord.x;
		float deltaV1 = v1.texCoord.y - v0.texCoord.y;
		float deltaU2 = v2.texCoord.x - v0.texCoord.x;
		float deltaV2 = v2.texCoord.y - v0.texCoord.y;

		float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);
		glm::vec3 tangent = f * (deltaV2 * edge1 - deltaV1 * edge2);
		
		v0.tangent += tangent;
		v1.tangent += tangent;
		v2.tangent += tangent;
	}
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


Buffer Renderer::CreateStagingBuffer(Size bufferSize) {

	Buffer stagingBuffer;
	stagingBuffer.bufferSize = bufferSize;
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
		frame.renderFence = pCore->CreateFence( vk::FenceCreateFlagBits::eSignaled);
		frame.renderSemaphore = pCore->CreateSemaphore();
		frame.presentSemaphore = pCore->CreateSemaphore();

		frame.commandPool = pCore->CreateCommandPool( pCore->QueueFamilies().graphicsFamily);
		frame.commandBuffer = pCore->AllocateCommandBuffer( frame.commandPool);

		mDeletionStack.Push( [&](){
			pCore->Device().destroyFence( frame.renderFence, nullptr);
			pCore->Device().destroySemaphore( frame.renderSemaphore, nullptr);
			pCore->Device().destroySemaphore( frame.presentSemaphore, nullptr);
			
			pCore->Device().destroyCommandPool( frame.commandPool);
		});
	}	
}


void Renderer::CreateSubmitContexts() {

	mGraphicsSubmit.fence = pCore->CreateFence();
	mGraphicsSubmit.commandPool = pCore->CreateCommandPool( pCore->QueueFamilies().graphicsFamily);
	mGraphicsSubmit.commandBuffer = pCore->AllocateCommandBuffer( mGraphicsSubmit.commandPool);

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


void Renderer::CreateSamplers() {

	mSampler = pCore->CreateSampler(
		vk::Filter::eLinear, vk::Filter::eLinear,
		vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
		false, 0.f, 
		vk::SamplerMipmapMode::eNearest
	);

	mDeletionStack.Push( [&](){
		pCore->Device().destroySampler( mSampler);
	});
}


void Renderer::CreateDescriptorResources() {

	mCameraBuffer.bufferSize = sizeof(CameraData);
	mCameraBuffer.usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst;
	mCameraBuffer.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mCameraBuffer.buffer = pCore->CreateBuffer( mCameraBuffer.bufferSize, mCameraBuffer.usage);
	mCameraBuffer.bufferMemory = pCore->AllocateBufferMemory( mCameraBuffer.buffer, mCameraBuffer.memoryType);

	mDeletionStack.Push( [&](){
		pCore->Device().destroyBuffer( mCameraBuffer.buffer);
		pCore->Device().freeMemory( mCameraBuffer.bufferMemory);
	});

	mSceneBuffer.bufferSize = sizeof(SceneData);
	mSceneBuffer.usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst;
	mSceneBuffer.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mSceneBuffer.buffer = pCore->CreateBuffer( mSceneBuffer.bufferSize, mSceneBuffer.usage);
	mSceneBuffer.bufferMemory = pCore->AllocateBufferMemory( mSceneBuffer.buffer, mSceneBuffer.memoryType);
	
	mDeletionStack.Push( [&](){
		pCore->Device().destroyBuffer( mSceneBuffer.buffer);
		pCore->Device().freeMemory( mSceneBuffer.bufferMemory);
	});

	CreateImage( mMaterialAlbedoImage, mResourceDir + "Materials/Rusted_Iron/rusted_iron_albedo.png", vk::Format::eR8G8B8A8Srgb);
	CreateImage( mMaterialNormalImage, mResourceDir + "Materials/Default/default_normal.png");
	CreateImage( mMaterialMetallicImage, mResourceDir + "Materials/Rusted_Iron/rusted_iron_metallic.png");
	CreateImage( mMaterialRoughnessImage, mResourceDir + "Materials/Rusted_Iron/rusted_iron_roughness.png");
	CreateImage( mMaterialAoImage, mResourceDir + "Materials/Default/default_white.png");

	mObjectBuffer.bufferSize = sizeof(ObjectData);
	mObjectBuffer.usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst;
	mObjectBuffer.memoryType = vk::MemoryPropertyFlagBits::eDeviceLocal;
	mObjectBuffer.buffer = pCore->CreateBuffer( mObjectBuffer.bufferSize, mObjectBuffer.usage);
	mObjectBuffer.bufferMemory = pCore->AllocateBufferMemory( mObjectBuffer.buffer, mObjectBuffer.memoryType);

	mDeletionStack.Push( [&](){
		pCore->Device().destroyBuffer( mObjectBuffer.buffer);
		pCore->Device().freeMemory( mObjectBuffer.bufferMemory);
	});
}


void Renderer::CreateImage(Image &image, std::string_view path, vk::Format format) {

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

	mDeletionStack.Push( [&](){
		pCore->Device().destroyImageView( image.imageView);
		pCore->Device().destroyImage( image.image);
		pCore->Device().freeMemory( image.imageMemory);
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
		.setStageFlags( vk::ShaderStageFlagBits::eFragment );
	
	vk::DescriptorSetLayoutBinding passBindings[] = {
		cameraDataBinding, sceneDataBinding
	};
	mPassDataLayout = pCore->CreateDescriptorSetLayout(2, passBindings);	
			
	auto albedoSamplerBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 0 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eFragment );
	auto normalSamplerBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 1 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eFragment );		
	auto metallicSamplerBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 2 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eFragment );	
	auto roughnessSamplerBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 3 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eFragment );	
	auto aoSamplerBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 4 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eFragment );

	vk::DescriptorSetLayoutBinding materialBindings[] = {
		albedoSamplerBinding, normalSamplerBinding, metallicSamplerBinding, roughnessSamplerBinding, aoSamplerBinding
	};
	mMaterialDataLayout = pCore->CreateDescriptorSetLayout(5, materialBindings);


	auto objectDataBinding = vk::DescriptorSetLayoutBinding{}
		.setBinding( 0 )
		.setDescriptorType( vk::DescriptorType::eUniformBuffer )
		.setDescriptorCount( 1 )
		.setStageFlags( vk::ShaderStageFlagBits::eVertex );
	
	mObjectDataLayout = pCore->CreateDescriptorSetLayout(1, &objectDataBinding);

	mDeletionStack.Push( [&](){
		pCore->Device().destroyDescriptorSetLayout( mPassDataLayout);
		pCore->Device().destroyDescriptorSetLayout( mObjectDataLayout);
		pCore->Device().destroyDescriptorSetLayout( mMaterialDataLayout); 
	});
}


void Renderer::CreateDescriptorPool() {

	vk::DescriptorPoolSize poolSizes[] = {

		{vk::DescriptorType::eUniformBuffer,  10 * pFrameOverlap->Get()},
		{vk::DescriptorType::eCombinedImageSampler,  10 * pFrameOverlap->Get()}
	};

	mDescriptorPool = pCore->CreateDescriptorPool(10*pFrameOverlap->Get(), 2, poolSizes);

	mDeletionStack.Push( [&](){
		pCore->Device().destroyDescriptorPool( mDescriptorPool);
	});
}


void Renderer::CreateDescriptorSets() {

	vk::DescriptorSetLayout descriptorSetLayouts[] = {
		mPassDataLayout, mMaterialDataLayout, mObjectDataLayout
	};
	auto descriptorSets = pCore->AllocateDescriptorSets(mDescriptorPool, 3, descriptorSetLayouts);

	mPassDataSet = descriptorSets[0];
	mMaterialDataSet = descriptorSets[1];
	mObjectDataSet = descriptorSets[2];


	auto cameraInfo = mCameraBuffer.DescriptorInfo();
	auto cameraWrite = vk::WriteDescriptorSet{}
		.setDstSet( mPassDataSet )
		.setDstBinding( 0 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eUniformBuffer )
		.setPBufferInfo( &cameraInfo );

	auto sceneInfo = mSceneBuffer.DescriptorInfo();
	auto sceneWrite = vk::WriteDescriptorSet{}
		.setDstSet( mPassDataSet )
		.setDstBinding( 1 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eUniformBuffer )
		.setPBufferInfo( &sceneInfo );

	auto materialAlbedoInfo = mMaterialAlbedoImage.DescriptorInfo( mSampler);
	auto materialAlbedoWrite = vk::WriteDescriptorSet{}
		.setDstSet( mMaterialDataSet )
		.setDstBinding( 0 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setPImageInfo( &materialAlbedoInfo );

	auto materialNormalInfo = mMaterialNormalImage.DescriptorInfo( mSampler);
	auto materialNormalWrite = vk::WriteDescriptorSet{}
		.setDstSet( mMaterialDataSet )
		.setDstBinding( 1 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setPImageInfo( &materialNormalInfo );

	auto materialMetallicInfo = mMaterialMetallicImage.DescriptorInfo( mSampler);
	auto materialMetallicWrite = vk::WriteDescriptorSet{}
		.setDstSet( mMaterialDataSet )
		.setDstBinding( 2 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setPImageInfo( &materialMetallicInfo );

	auto materialRoughnessInfo = mMaterialRoughnessImage.DescriptorInfo( mSampler);
	auto materialRoughnessWrite = vk::WriteDescriptorSet{}
		.setDstSet( mMaterialDataSet )
		.setDstBinding( 3 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setPImageInfo( &materialRoughnessInfo );
		
	auto materialAoInfo = mMaterialAoImage.DescriptorInfo( mSampler);
	auto materialAoWrite = vk::WriteDescriptorSet{}
		.setDstSet( mMaterialDataSet )
		.setDstBinding( 4 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
		.setPImageInfo( &materialAoInfo );

	auto objectInfo = mObjectBuffer.DescriptorInfo();
	auto objectWrite = vk::WriteDescriptorSet{}
		.setDstSet( mObjectDataSet )
		.setDstBinding( 0 )
		.setDstArrayElement( 0 )
		.setDescriptorCount( 1 )
		.setDescriptorType( vk::DescriptorType::eUniformBuffer )
		.setPBufferInfo( &objectInfo );	

	vk::WriteDescriptorSet descriptorWrites[] = {
		cameraWrite, sceneWrite, materialAlbedoWrite, materialNormalWrite, materialMetallicWrite, materialRoughnessWrite, materialAoWrite, objectWrite
	};
	pCore->Device().updateDescriptorSets(8, descriptorWrites, 0, nullptr);
}


void Renderer::CreateShaderModules() {

	auto vertShaderCode = mFiles.Read( mResourceDir + "Shaders/single_mesh_vert.spv" , std::ios::binary);
	auto fragShaderCode = mFiles.Read( mResourceDir + "Shaders/single_material_pbr_frag.spv" , std::ios::binary);

	mMeshVertShader = pCore->CreateShaderModule( vertShaderCode.GetSize(), vertShaderCode.Data() );
	mMaterialFragShader = pCore->CreateShaderModule( fragShaderCode.GetSize(), fragShaderCode.Data() );

	mDeletionStack.Push( [&](){
		pCore->Device().destroyShaderModule( mMeshVertShader);
		pCore->Device().destroyShaderModule( mMaterialFragShader);
	});
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
		.setCullMode( vk::CullModeFlagBits::eBack )
		.setFrontFace( vk::FrontFace::eCounterClockwise)
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
		mPassDataLayout, mMaterialDataLayout, mObjectDataLayout
	};
	mSingleMaterialPipeline.pipelineLayout = pCore->CreatePipelineLayout( 3, descriptorSetLayouts );

	// render pass
	mSingleMaterialPipeline.renderpass = mForwardPass;
	mSingleMaterialPipeline.subpass = 0;

	pCore->CreatePipeline( mSingleMaterialPipeline );

	mDeletionStack.Push( [&](){
		pCore->Device().destroyPipeline( mSingleMaterialPipeline.pipeline);
		pCore->Device().destroyPipelineLayout( mSingleMaterialPipeline.pipelineLayout);
	});
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
	UpdateScenedata();
	UpdateObjectData();


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


	// get current frames command buffer and reset
	auto &cmd = frame.commandBuffer;
	cmd.reset( vk::CommandBufferResetFlagBits::eReleaseResources);	


	// record render commands
	auto cmdBeginInfo = vk::CommandBufferBeginInfo{}
		.setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );		
	cmd.begin(cmdBeginInfo);	

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


	cmd.bindPipeline(
		vk::PipelineBindPoint::eGraphics, mSingleMaterialPipeline.pipeline
	);

	vk::Buffer vertexBuffers[] = {mCombinedVertexBuffer.buffer};
	Size offsets[] = {0};
	cmd.bindVertexBuffers( 0, 1, vertexBuffers, offsets);
	cmd.bindIndexBuffer( mCombinedIndexBuffer.buffer, 0, vk::IndexType::eUint32);

		
	vk::DescriptorSet descriptorSets[] = {
		mPassDataSet, mMaterialDataSet, mObjectDataSet
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


void Renderer::UpdateCameraData() {

	CameraData camera;
	camera.position = {2.f, 3.f, 4.f};
	// float angle = (float)mFrameCount / 120.f * glm::radians(30.f);
	// camera.position = {4.f*cos(angle), 0.f, 4.f*sin(angle)};
	camera.view = glm::lookAt(
		camera.position,
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f)
	);
	camera.proj = glm::perspective(
		glm::radians(60.f),
		(float)mSwapchain.extent.width / (float)mSwapchain.extent.height,
		0.1f, 100.0f
	);
	camera.proj[1][1] *= -1; // flip y scaling factor -> correction of OpenGL inverse y-axis
	camera.viewProj = camera.proj * camera.view;
	
	UploadBufferData( &camera, sizeof(CameraData), mCameraBuffer.buffer);
}


void Renderer::UpdateScenedata() {

	float angle = (float)mFrameCount / 120.f * glm::radians(30.f);
	glm::vec3 direction = {cos(angle), 0.f, sin(angle)};
	SceneData scene;
	scene.ambient = {
		glm::vec3(1.f, 1.f, 1.f),
		0.03f
	};
	scene.light = {
		glm::vec3(1.f, 1.f, 1.f),
		2.5f,
		direction
	};

	UploadBufferData( &scene, sizeof(SceneData), mSceneBuffer.buffer);
}


void Renderer::UpdateObjectData() {

	glm::mat4 rotation = glm::rotate(
		glm::mat4(1.f), 
		(float)mFrameCount / 60.f * glm::radians(30.f), 
		glm::vec3(0.5f, 1.f, 0.75f)
	);
	glm::mat4 translate = glm::translate(
		glm::mat4(1.f),
		glm::vec3(0.f, 0.f, 0.f)
	);
	glm::mat4 scale = glm::scale(
		glm::mat4(1.f),
		glm::vec3(1.f, 1.f, 1.f)
	);
	ObjectData obj;
	obj.transform = translate * rotation * scale;
	
	UploadBufferData( &obj, sizeof(ObjectData), mObjectBuffer.buffer);
}

    
} // Atuin
