
#include "Descriptors.h"
#include "Core/Config/ConfigManager.h"


namespace Atuin {


CVar<U32>* DescriptorSetAllocator::pMaxSetsPerPool = ConfigManager::RegisterCVar( "Rednerer", "MAX_DESCRIPTOR_SETS", (U32)100);


/***    DescriptorSetAllocator    ****/


void DescriptorSetAllocator::Init( vk::Device device) {

    mDevice = device;
    U32 maxSetsPerPool = pMaxSetsPerPool->Get();
    mPoolSizes = {
        // vk::DescriptorPoolSize{ vk::DescriptorType::eSampler,              (U32)( (float)maxSetsPerPool * 0.5f) },
		// vk::DescriptorPoolSize{ vk::DescriptorType::eSampledImage,         maxSetsPerPool * 4 },
        vk::DescriptorPoolSize{ vk::DescriptorType::eCombinedImageSampler, maxSetsPerPool * 4 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eStorageImage,         maxSetsPerPool * 4 },
		// vk::DescriptorPoolSize{ vk::DescriptorType::eUniformTexelBuffer,   maxSetsPerPool * 1 },
		// vk::DescriptorPoolSize{ vk::DescriptorType::eStorageTexelBuffer,   maxSetsPerPool * 1 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer,        maxSetsPerPool * 2 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBufferDynamic, maxSetsPerPool * 1 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBuffer,        maxSetsPerPool * 4 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBufferDynamic, maxSetsPerPool * 1 },
		// vk::DescriptorPoolSize{ vk::DescriptorType::eInputAttachment,      (U32)( (float)maxSetsPerPool * 0.5f) }
    };
}
    

vk::DescriptorSet DescriptorSetAllocator::Allocate( vk::DescriptorSetLayout layout) {

    if ( !mCurrPool)
    {
        mCurrPool = GetNewPool();
        mUsedPools.PushBack( mCurrPool);
    }

    auto allocInfo = vk::DescriptorSetAllocateInfo{}
		.setDescriptorPool( mCurrPool )
		.setDescriptorSetCount( 1 )
		.setPSetLayouts( &layout );

	vk::DescriptorSet descriptorSet;
	vk::Result result = mDevice.allocateDescriptorSets( &allocInfo, &descriptorSet);
    if (result == vk::Result::eErrorFragmentedPool || result == vk::Result::eErrorOutOfPoolMemory)
    {
        mCurrPool = GetNewPool();
        mUsedPools.PushBack( mCurrPool);
        result = mDevice.allocateDescriptorSets( &allocInfo, &descriptorSet);
    }

	if(result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to allocate descriptor set " + vk::to_string(result));
	}

	return descriptorSet;
}


void DescriptorSetAllocator::ResetPools() {

    for( auto pool : mUsedPools)
    {
        mDevice.resetDescriptorPool( pool);
        mFreePools.PushBack( pool);
    }
    mUsedPools.Clear();
    mCurrPool = VK_NULL_HANDLE;
}


void DescriptorSetAllocator::DestroyPools() {

    for( auto pool : mFreePools) 
    {
        mDevice.destroyDescriptorPool( pool);
    }
    for( auto pool : mUsedPools) 
    {
        mDevice.destroyDescriptorPool( pool);
    }
}


vk::DescriptorPool DescriptorSetAllocator::GetNewPool() {

    if( mFreePools.IsEmpty())
    {
        return CreateDescriptorPool( pMaxSetsPerPool->Get(), (U32)mPoolSizes.GetSize(), mPoolSizes.Data());
    }

    auto pool = mFreePools.Back();
    mFreePools.PopBack();

    return pool;
}


vk::DescriptorPool DescriptorSetAllocator::CreateDescriptorPool( 
    U32 maxSets, 
    U32 count, vk::DescriptorPoolSize* poolSizes, 
    vk::DescriptorPoolCreateFlags flags) 
{
    auto poolInfo = vk::DescriptorPoolCreateInfo{}
		.setMaxSets( maxSets )
		.setPoolSizeCount( count )
		.setPPoolSizes( poolSizes )
		.setFlags( flags );
		
	vk::DescriptorPool descriptorPool;
	vk::Result result = mDevice.createDescriptorPool(&poolInfo, nullptr, &descriptorPool);
	if( result != vk::Result::eSuccess )
	{
		throw std::runtime_error("Failed to create descriptor pool " + vk::to_string(result));
	}

	return descriptorPool;
}


/***    DescriptorLayoutCache    ****/


void DescriptorLayoutCache::Init( vk::Device device) {

    mDevice = device;
}


vk::DescriptorSetLayout DescriptorLayoutCache::CreateLayout( vk::DescriptorSetLayoutCreateInfo *layoutInfo) {

    U64 layoutId = 0;
    for (U32 i = 0; i < layoutInfo->bindingCount; i++)
    {
        auto binding = layoutInfo->pBindings[i];
        layoutId = (layoutId << 1) ^ ( (size_t)binding.binding << 32 | (uint32_t)binding.descriptorType << 16 |(uint32_t)binding.stageFlags );
    }

    if ( mLayouts.Find( layoutId) == mLayouts.End())
    {
        vk::DescriptorSetLayout newLayout;
        vk::Result result = mDevice.createDescriptorSetLayout(layoutInfo, nullptr, &newLayout);
        if ( result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create descriptor set layout " + vk::to_string(result));
        }
        
        mLayouts[layoutId] = newLayout;
    }

    return mLayouts[layoutId];
}


void DescriptorLayoutCache::DestroyLayouts() {

    for ( auto &[info, layout] : mLayouts)
    {
        mDevice.destroyDescriptorSetLayout( layout);
    }
}



/***    DescriptorBuilder    ****/


DescriptorSetBuilder& DescriptorSetBuilder::BindBuffer( U32 binding, vk::DescriptorType type, vk::ShaderStageFlags stages, vk::DescriptorBufferInfo* bufferInfo) {

    auto newBinding = vk::DescriptorSetLayoutBinding{}
        .setBinding( binding)
        .setDescriptorCount(1)
        .setDescriptorType( type)
        .setStageFlags( stages);
    mBindings.PushBack( newBinding);

	auto newWrite = vk::WriteDescriptorSet{}
		.setDstBinding( binding )
		.setDescriptorCount( 1 )
		.setPBufferInfo( bufferInfo )
		.setDescriptorType( type );
        // set dest set in Build()
    mWrites.PushBack( newWrite);
    
    return *this;
}


DescriptorSetBuilder& DescriptorSetBuilder::BindImage( U32 binding, vk::DescriptorType type, vk::ShaderStageFlags stages, vk::DescriptorImageInfo* imageInfo) {

    auto newBinding = vk::DescriptorSetLayoutBinding{}
        .setBinding( binding)
        .setDescriptorCount(1)
        .setDescriptorType( type)
        .setStageFlags( stages);
    mBindings.PushBack( newBinding);

	auto newWrite = vk::WriteDescriptorSet{}
		.setDstBinding( binding )
		.setDescriptorCount( 1 )
		.setPImageInfo( imageInfo )
		.setDescriptorType( type );
        // set dest set in Build()
    mWrites.PushBack(newWrite);

    return *this;
}


vk::DescriptorSet DescriptorSetBuilder::Build() {

    auto layoutCreateInfo = vk::DescriptorSetLayoutCreateInfo{}
        .setBindingCount( (U32)mBindings.GetSize())
        .setPBindings( mBindings.Data());

    vk::DescriptorSetLayout layout = pLayoutCache->CreateLayout( &layoutCreateInfo);

    vk::DescriptorSet descriptorSet = pAllocator->Allocate(layout);
    for ( auto &write : mWrites) 
    {
        write.setDstSet( descriptorSet );
    }
    mDevice.updateDescriptorSets( (U32)mWrites.GetSize(), mWrites.Data(), 0, nullptr);   

    return descriptorSet;
}


} // namespace Atuin
