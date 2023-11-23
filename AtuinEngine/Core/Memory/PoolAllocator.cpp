
#include "PoolAllocator.h"
#include "Core/Util/StringFormat.h"

#include "stdexcept"


namespace Atuin {


PoolAllocator::PoolAllocator(Size numChunks, Size chunkSize, IAllocator *parent) :
    IAllocator(numChunks * chunkSize, parent),
    mNumChunks {numChunks},
    mChunkSize {chunkSize}
{
    CreatePool();
}


PoolAllocator::~PoolAllocator() {

}


void* PoolAllocator::Allocate() {

    return Allocate(mChunkSize, 1);
}


void* PoolAllocator::Allocate(Size size, U8 alignment) {

    assert(size > 0 && size <= mChunkSize);
    assert(mChunkSize % alignment == 0);

    if(!pHead)
    {
        throw std::overflow_error( FormatStr("Pool allocator does not have a large enough memory region available for allocation of size %i. Free space %i.", size, mTotalMemory - mUsedMemory));
    }

    void *mem = reinterpret_cast<void*>(pHead);
    pHead = pHead->next;
    
    mUsedMemory += mChunkSize;
    mMaxUsedMemory = std::max(mMaxUsedMemory, mUsedMemory);

    return mem;
}


void PoolAllocator::Free(void *ptr) {

    assert(ptr != nullptr);

    pHead = new (ptr) PoolNode{pHead};
    mUsedMemory -= mChunkSize;
}


void PoolAllocator::Clear() {

    CreatePool();
    mUsedMemory = 0;
}


void PoolAllocator::CreatePool() {

    pHead = nullptr;
    UPtr top = reinterpret_cast<UPtr>(pBase) + mTotalMemory;
    for (Size i = 1; i <= mNumChunks; i++) 
    {
        pHead = new ( reinterpret_cast<void*>(top - i*mChunkSize) ) PoolNode{pHead};
    }
}

    
} // Atuin
