
#include "Pipeline.h"
#include "Mesh.h"


namespace Atuin {


void CreatePipelineLayout( vk::Device device, const Array<vk::DescriptorSetLayout> &descriptorLayouts, const Array<vk::PushConstantRange> &pushConstants, vk::PipelineLayout *pipelineLayout) {

	auto layoutInfo = vk::PipelineLayoutCreateInfo{}
		.setSetLayoutCount( (U32)descriptorLayouts.GetSize() )
		.setPSetLayouts( descriptorLayouts.Data() )
		.setPushConstantRangeCount( (U32)pushConstants.GetSize() )
		.setPPushConstantRanges( pushConstants.Data() );

	vk::Result result = device.createPipelineLayout(&layoutInfo, nullptr, pipelineLayout);
	if ( result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create pipeline layout : " + vk::to_string(result));
	}
}


Pipeline GraphicsPipelineBuilder::Build( vk::Device device, vk::RenderPass renderpass) {

    Pipeline pipeline;
	pipeline.type = Pipeline::Type::GRAPHICS;

	CreatePipelineLayout(device, descriptorLayouts, pushConstants, &pipeline.pipelineLayout);

    auto pipelineInfo = vk::GraphicsPipelineCreateInfo{}
		.setStageCount( (U32)shaderInfos.GetSize() )
		.setPStages( shaderInfos.Data() )
		.setPVertexInputState( &vertexInputInfo )
		.setPTessellationState( &tesselationInfo)
		.setPInputAssemblyState( &inputAssemblyInfo )
		.setPViewportState( &viewportInfo )
		.setPRasterizationState( &rasterizerInfo )
		.setPMultisampleState( &multisampleInfo )
		.setPDepthStencilState( &depthStencilInfo )
		.setPColorBlendState( &colorBlendInfo )
		.setPDynamicState( &dynamicStateInfo )
		.setLayout( pipeline.pipelineLayout )
		.setRenderPass( renderpass )
		.setSubpass( 0 )
		.setBasePipelineHandle( VK_NULL_HANDLE )
		.setBasePipelineIndex( -1 );
	
	vk::Result result = device.createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &pipeline.pipeline);
	if(result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create graphics pipeline : " + vk::to_string(result));
	}

    return pipeline;
}


void GraphicsPipelineBuilder::FillFromJson( Json &pipelineJson) {

	// vertex input
	Json &disableVertexInput = pipelineJson[ "disableVertexInput"];
	if ( !disableVertexInput.IsNull() && disableVertexInput.ToBool())
	{		
		vertexInputInfo
			.setVertexBindingDescriptionCount( 0 )
			.setPVertexBindingDescriptions( nullptr )
			.setVertexAttributeDescriptionCount( 0 )
			.setPVertexAttributeDescriptions( nullptr );
	}

	// tesselation
	Json &tesselationPoints = pipelineJson[ "tesselationControlPoints"];
	if ( !tesselationPoints.IsNull())
	{
		tesselationInfo.setPatchControlPoints( (U32)tesselationPoints.ToInt());
	}

	// rasterization
	Json &rasterization = pipelineJson[ "rasterization"];
	if ( !rasterization.IsNull())
	{
		// rasterizer discard
		Json &discard = rasterization[ "discard"];
		if ( !discard.IsNull())
		{
			rasterizerInfo.setRasterizerDiscardEnable( discard.ToBool() );
		}		
		// polygon mode
		Json &polygonMode = rasterization[ "polygonMode"];
		if( !polygonMode.IsNull())
		{
			std::string polygonModeStr = polygonMode.ToString();
			if ( polygonModeStr == "point")
			{
				rasterizerInfo.setPolygonMode( vk::PolygonMode::ePoint );
			}
			else if ( polygonModeStr == "line")
			{
				rasterizerInfo.setPolygonMode( vk::PolygonMode::eLine );
			}
			else if ( polygonModeStr == "fill")
			{
				rasterizerInfo.setPolygonMode( vk::PolygonMode::eFill );
			}
		}
		// culling ( either back face culling or none )
		Json &culling = rasterization[ "culling"];
		if ( !culling.IsNull())
		{
			rasterizerInfo.setCullMode( culling.ToBool() ? vk::CullModeFlagBits::eBack : vk::CullModeFlagBits::eNone );
		}
		// line width
		Json &lineWidth = rasterization[ "lineWidth"];
		if ( !lineWidth.IsNull())
		{
			rasterizerInfo.setLineWidth( (float)lineWidth.ToFloat() );
		}

		// depth bias
		Json &depthBias = rasterization[ "depthBias"];
		if ( !depthBias.IsNull())
		{
			auto &depthBiasData = depthBias.GetList();
			rasterizerInfo.depthBiasEnable = true;
			rasterizerInfo.depthBiasConstantFactor = (float)depthBiasData[0].ToFloat();
			rasterizerInfo.depthBiasSlopeFactor    = (float)depthBiasData[1].ToFloat();
			rasterizerInfo.depthBiasClamp          = (float)depthBiasData[2].ToFloat();
		}
	}
	
	// depth stencil
	auto depthStencil = pipelineJson[ "depthStencil"];
	if ( !depthStencil.IsNull())
	{
		// depth test 
		auto enableTest = depthStencil[ "enableDepthTest"];
		if ( !enableTest.IsNull())
		{
			depthStencilInfo.setDepthTestEnable( enableTest.ToBool() );
		}
		// depth test 
		auto enableWrite = depthStencil[ "enableDepthWrite"];
		if ( !enableWrite.IsNull())
		{
			depthStencilInfo.setDepthWriteEnable( enableWrite.ToBool() );
		}	
	}
	
}


Pipeline ComputePipelineBuilder::Build( vk::Device device) {

    Pipeline pipeline;
	pipeline.type = Pipeline::Type::COMPUTE;

	CreatePipelineLayout( device, descriptorLayouts, pushConstants, &pipeline.pipelineLayout);

    auto pipelineInfo = vk::ComputePipelineCreateInfo{}
        .setStage( shaderInfo)
        .setLayout( pipeline.pipelineLayout);
	
	vk::Result result = device.createComputePipelines( nullptr, 1, &pipelineInfo, nullptr, &pipeline.pipeline);
	if(result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to create compute pipeline : " + vk::to_string(result));
	}

    return pipeline;
}

    
} // Atuin
