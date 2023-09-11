
#pragma once


#include "Core/Util/Types.h"
#include "Core/DataStructures/Array.h"
#include "Core/DataStructures/Map.h"

#include "vulkan/vulkan.hpp"


namespace Atuin {




class DescriptorSetAllocator {

public:

    void Init( vk::Device device);
    vk::DescriptorSet Allocate( vk::DescriptorSetLayout layout);
    void ResetPools();
    void DestroyPools();


private:

    vk::DescriptorPool GetNewPool();
    vk::DescriptorPool CreateDescriptorPool( U32 maxSets, U32 count, vk::DescriptorPoolSize* poolSizes, vk::DescriptorPoolCreateFlags flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

    vk::Device mDevice;

    U32 mMaxSetsPerPool; // TODO maybe as CVar
    Array<vk::DescriptorPoolSize> mPoolSizes;

    vk::DescriptorPool mCurrPool;
    Array<vk::DescriptorPool> mUsedPools;
    Array<vk::DescriptorPool> mFreePools;

};


class DescriptorSetBuilder {

public:

    DescriptorSetBuilder( vk::Device device, DescriptorSetAllocator *allocator) : mDevice {device}, pAllocator {allocator} {}

    DescriptorSetBuilder& BindBuffer( U32 binding, vk::DescriptorBufferInfo* bufferInfo, vk::DescriptorType type);
	DescriptorSetBuilder& BindImage( U32 binding, vk::DescriptorImageInfo* imageInfo, vk::DescriptorType type);

	vk::DescriptorSet Build( vk::DescriptorSetLayout layout);

private:

    vk::Device mDevice;
    DescriptorSetAllocator* pAllocator;

    Array<vk::WriteDescriptorSet> mWrites;
};


} // namespace Atuin