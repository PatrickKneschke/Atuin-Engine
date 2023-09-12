
#pragma once


#include "Core/Util/Types.h"
#include "Core/Util/StringFormat.h"

#include "vulkan/vulkan.hpp"


namespace Atuin {

namespace Sampler{


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
		throw std::runtime_error( FormatStr( "Unsupported or unknown filter setting \"%s\"", filter));
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
		throw std::runtime_error( FormatStr( "Unsupported or unknown address mode setting \"%s\"", addressMode));
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
		throw std::runtime_error( FormatStr( "Unsupported or unknown mipmap mode setting \"%s\"", mipmapMode));
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
		throw std::runtime_error( FormatStr( "Unsupported or unknown compare operation setting \"%s\"", compareOp));
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
		throw std::runtime_error( FormatStr( "Unsupported or unknown border color setting \"%s\"", borderColor));
	}

    return res;
}


} // Sampler
    
} // Atuin
