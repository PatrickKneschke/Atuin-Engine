
#include "Renderpass.h"


namespace Atuin {


void Renderpass::CreateRenderPass( vk::Device device, Array<vk::AttachmentDescription> &attachments, I32 depthAttachmentIdx) {

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

	vk::Result result = device.createRenderPass(&renderPassInfo, nullptr, &renderpass);	
	if(result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create render pass : "+ vk::to_string(result));
	}
}


void Renderpass::CreateFramebuffers( vk::Device device, U32 imageWidth, U32 imageHeight, Array<Array<vk::ImageView>> &attachments) {

    U32 imageCount = (U32)attachments.GetSize();
    framebuffers.Resize(imageCount);
	for(Size i=0; i < imageCount; i++)
	{
        auto framebufferInfo = vk::FramebufferCreateInfo{}
            .setRenderPass( renderpass )
            .setAttachmentCount( (U32)attachments[i].GetSize() )
            .setPAttachments( attachments[i].Data() )
            .setWidth( imageWidth )
            .setHeight( imageHeight )
            .setLayers( 1 );

        vk::Result result = device.createFramebuffer(&framebufferInfo, nullptr, &framebuffers[i]);	
        if(result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create framebuffer : "+ vk::to_string(result));
        }	
	}

}

    
} // Atuin
