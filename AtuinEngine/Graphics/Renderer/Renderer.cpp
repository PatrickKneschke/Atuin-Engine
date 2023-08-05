
#include "Renderer.h"
#include "RendererCore.h"
#include "Core/Util/Types.h"

#include "GLFW/glfw3.h"


namespace Atuin {


Renderer::Renderer() : 
	mLog(), 
	mMemory(), 
	mJobs(), 
	pWindow {nullptr}, 
	pCore {nullptr} 
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
}


void Renderer::ShutDown() {

	for (auto imageView : mSwapchain.imageViews)
	{
		pCore->Device().destroyImageView( imageView);
	}
	pCore->Device().destroySwapchainKHR( mSwapchain.swapchain, nullptr);


	mMemory.Delete(pCore);
}
    

void Renderer::Update() {


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

    
} // Atuin
