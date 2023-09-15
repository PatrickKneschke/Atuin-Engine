
#pragma once


#include "VulkanInclude.h"
#include "Core/Util/Types.h"
#include "Core/Util/StringFormat.h"



namespace Atuin {

namespace JsonVk{


inline vk::Format GetFormat( std::string_view format) {

	vk::Format res;
	if ( format == "unorm")
	{
		res = vk::Format::eR8G8B8A8Unorm;
	}
	else if ( format == "srgb")
	{
		res = vk::Format::eR8G8B8A8Srgb;
	}
	else 
	{
		throw std::runtime_error( FormatStr( "Unsupported or unknown format \"%s\"", format));
	}

	return res;
}


inline vk::Filter GetFilter( std::string_view filter) {

    vk::Filter res;
    if ( filter == "linear")
	{
		res = vk::Filter::eLinear;
	}
	else if ( filter == "nearest")
	{
		res = vk::Filter::eNearest;
	}
	else
	{
		throw std::runtime_error( FormatStr( "Unsupported or unknown filter \"%s\"", filter));
	}

    return res;
}


inline vk::SamplerAddressMode GetAddressMode( std::string_view addressMode) {

    vk::SamplerAddressMode res;
    if ( addressMode == "clamp")
	{
		res = vk::SamplerAddressMode::eClampToBorder;
	}
	else if ( addressMode == "repeat")
	{
		res = vk::SamplerAddressMode::eRepeat;
	}
    else if ( addressMode == "mirror")
	{
		res = vk::SamplerAddressMode::eMirroredRepeat;
	}
	else
	{
		throw std::runtime_error( FormatStr( "Unsupported or unknown address mode \"%s\"", addressMode));
	}

    return res;
}


inline vk::SamplerMipmapMode GetMipMapMode( std::string_view mipmapMode) {

    vk::SamplerMipmapMode res;
    if ( mipmapMode == "linear")
	{
		res = vk::SamplerMipmapMode::eLinear;
	}
	else if ( mipmapMode == "nearest")
	{
		res = vk::SamplerMipmapMode::eNearest;
	}
	else
	{
		throw std::runtime_error( FormatStr( "Unsupported or unknown mipmap mode \"%s\"", mipmapMode));
	}

    return res;
}


inline vk::CompareOp GetCompareOp( std::string_view compareOp) {

    vk::CompareOp res;
    if ( compareOp == "never")
	{
		res = vk::CompareOp::eNever;
	}
    else if ( compareOp == "always")
	{
		res = vk::CompareOp::eAlways;
	}
    else if ( compareOp == "equal")
	{
		res = vk::CompareOp::eEqual;
	}
    else if ( compareOp == "not_equal")
	{
		res = vk::CompareOp::eNotEqual;
	}
    else if ( compareOp == "less")
	{
		res = vk::CompareOp::eLess;
	}
    else if ( compareOp == "less_equal")
	{
		res = vk::CompareOp::eLessOrEqual;
	}
    else if ( compareOp == "greater")
	{
		res = vk::CompareOp::eGreater;
	}
    else if ( compareOp == "greater_equal")
	{
		res = vk::CompareOp::eGreaterOrEqual;
	}
	else
	{
		throw std::runtime_error( FormatStr( "Unsupported or unknown compare operation \"%s\"", compareOp));
	}

    return res;
}


inline vk::BorderColor GetBorder( std::string_view borderColor) {

    vk::BorderColor res;
    if ( borderColor == "opaque_black")
	{
		res = vk::BorderColor::eFloatOpaqueBlack;
	}
	else if ( borderColor == "opaque_white")
	{
		res = vk::BorderColor::eFloatOpaqueWhite;
	}
	else
	{
		throw std::runtime_error( FormatStr( "Unsupported or unknown border color \"%s\"", borderColor));
	}

    return res;
}


inline vk::DescriptorType GetDescriptorType( std::string_view descriptorType) {

	vk::DescriptorType res;
	if ( descriptorType == "uniform_buffer")
	{
		res = vk::DescriptorType::eUniformBuffer;
	}
	else if ( descriptorType == "dynamic_uniform_buffer")
	{
		res = vk::DescriptorType::eUniformBufferDynamic;
	}
	else if ( descriptorType == "storage_buffer")
	{
		res = vk::DescriptorType::eStorageBuffer;
	}
	else if ( descriptorType == "dynamic_storage_buffer")
	{
		res = vk::DescriptorType::eStorageBufferDynamic;
	}
	else if ( descriptorType == "sampled_image")
	{
		res = vk::DescriptorType::eCombinedImageSampler;
	}
	else
	{
		throw std::runtime_error( FormatStr( "Unsupported or unknown descriptor type \"%s\"", descriptorType));
	}

	return res;
}


inline vk::ShaderStageFlagBits GetShaderStage( std::string_view stageFlag) {

	vk::ShaderStageFlagBits res;
	if (stageFlag == "vertex")
	{
		res = vk::ShaderStageFlagBits::eVertex;
	}
	else if (stageFlag == "geometry")
	{
		res = vk::ShaderStageFlagBits::eGeometry;
	}
	else if (stageFlag == "tesselation_ctrl")
	{
		res = vk::ShaderStageFlagBits::eTessellationControl;
	}
	else if (stageFlag == "tesselation_eval")
	{
		res = vk::ShaderStageFlagBits::eTessellationEvaluation;
	}
	else if (stageFlag == "fragment")
	{
		res = vk::ShaderStageFlagBits::eFragment;
	}	
	else if (stageFlag == "compute")
	{
		res = vk::ShaderStageFlagBits::eCompute;
	}
	else
	{
		throw std::runtime_error( FormatStr( "Unsupported or unknown shader stage \"%s\"", stageFlag));
	}

	return res;
}


} // JsonVk

    
} // Atuin
