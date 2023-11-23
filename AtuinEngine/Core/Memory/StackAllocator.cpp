
#include "StackAllocator.h"
#include "Core/Util/StringFormat.h"

#include "stdexcept"


namespace Atuin {


StackAllocator::StackAllocator(Size totalMemory, IAllocator *parent) :
    IAllocator(totalMemory, parent)
{
    mBaseAddress = reinterpret_cast<UPtr>(pBase);
    mTopAddress = mBaseAddress;
}


StackAllocator::~StackAllocator() {

}


void* StackAllocator::Allocate(Size size, U8 alignment) {

    assert(size > 0);

    Size adjustment = GetAlignmentAdjustment(mTopAddress, alignment);

    UPtr alignedAddress = mTopAddress + adjustment;
    if (alignedAddress + size - mBaseAddress > mTotalMemory)
    {
        throw std::overflow_error( FormatStr("Stack allocator does not have a large enough memory region available for allocation of size %i. Free space %i.", size, mTotalMemory - mUsedMemory));
    }

    mTopAddress = alignedAddress + size;
    mUsedMemory = mTopAddress - mBaseAddress;
    mMaxUsedMemory = std::max(mMaxUsedMemory, mUsedMemory);

    return reinterpret_cast<void*>(alignedAddress);    
}

    
void StackAllocator::Free(void *ptr) {

    assert(ptr != nullptr);

    UPtr newTopAddress = reinterpret_cast<UPtr>(ptr);

    // Do nothing if attempt is made to free memory outside used memory range
    if (mTopAddress <= newTopAddress)
    {
        return;
    }

    mTopAddress = newTopAddress;
    mUsedMemory = mTopAddress - mBaseAddress;
}


void StackAllocator::Clear() {

    mTopAddress = mBaseAddress;
    mUsedMemory = 0;
}

    
} // Atuin
