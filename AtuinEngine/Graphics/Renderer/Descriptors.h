
#pragma once


#include "VulkanInclude.h"
#include "Definitions.h"
#include "Core/Config/CVar.h"
#include "Core/Util/Types.h"
#include "Core/DataStructures/Array.h"
#include "Core/DataStructures/Map.h"


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


    static CVar<U32>* pMaxSetsPerPool;

    vk::Device mDevice;

    Array<vk::DescriptorPoolSize> mPoolSizes;

    vk::DescriptorPool mCurrPool;
    Array<vk::DescriptorPool> mUsedPools;
    Array<vk::DescriptorPool> mFreePools;

};


class DescriptorLayoutCache {

public:

    void Init( vk::Device device);
    vk::DescriptorSetLayout CreateLayout( vk::DescriptorSetLayoutCreateInfo *layoutInfo);
    void DestroyLayouts();

private:

    vk::Device mDevice;
    Map<U64, vk::DescriptorSetLayout> mLayouts;
};


class DescriptorSetBuilder {

public:

    DescriptorSetBuilder( vk::Device device, DescriptorSetAllocator *allocator, DescriptorLayoutCache *layoutCache) : mDevice {device}, pAllocator {allocator}, pLayoutCache {layoutCache} {}

    DescriptorSetBuilder& BindBuffer( U32 binding, vk::DescriptorType type, vk::ShaderStageFlags stages, vk::DescriptorBufferInfo* bufferInfo);
	DescriptorSetBuilder& BindImage( U32 binding, vk::DescriptorType type, vk::ShaderStageFlags stages, vk::DescriptorImageInfo* imageInfo);

	vk::DescriptorSet Build();

private:

    vk::Device mDevice;
    DescriptorSetAllocator* pAllocator;
    DescriptorLayoutCache *pLayoutCache;

    Array<vk::DescriptorSetLayoutBinding> mBindings;
    Array<vk::WriteDescriptorSet> mWrites;
};


} // namespace Atuin
