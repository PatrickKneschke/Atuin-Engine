
#include "FreeListAllocator.h"
#include "Core/Util/StringFormat.h"

#include <stdexcept>


namespace Atuin {


FreeListAllocator::FreeListAllocator(Size totalMemory, IAllocator *parent) :
    IAllocator(totalMemory, parent)
{
    pHead = new (pBase) FreeNode{mTotalMemory};
}


FreeListAllocator::~FreeListAllocator() {

}


void* FreeListAllocator::Allocate(Size size, U8 alignment) {

    assert(size > 0);

    // lock mutex
    const std::lock_guard<std::mutex> lock( mMutex);
   
    // Pad size so that total allocated space can fit a FreeNode when freed
    Size paddedSize = std::max(size, sizeof(FreeNode) - sizeof(AllocHeader));

    // Find memory region large enough for allocation
    Size requiredSize = paddedSize + sizeof(AllocHeader) + alignment - 1;
    FreeNode *currNode = pHead, *prevNode = nullptr;
    while (currNode && currNode->size < requiredSize)
    {
        prevNode = currNode;
        currNode = currNode->next;
    }

    if (currNode == nullptr)
    {
        throw std::overflow_error( FormatStr("Free list allocator does not have a large enough memory region available for allocation of size %i. Free space %i.", size, mTotalMemory - mUsedMemory));
    }

    // Find properly aligned address for allocation
    Size adjustment = GetAlignmentAdjustment(currNode->address + sizeof(AllocHeader), alignment);
    UPtr alignedAddress = currNode->address + adjustment + sizeof(AllocHeader);

    // Create a new node from the remaining memory region of the current node. 
    // If the remaining memory is smaller than a FreeNode add it to the allocated memory section instead.
    Size newSize = currNode->address + currNode->size - alignedAddress - paddedSize;
    Size allocSize = paddedSize;
    FreeNode *newNode = nullptr;
    if(newSize >= sizeof(FreeNode))
    {
        newNode = new (reinterpret_cast<void*>(alignedAddress + paddedSize)) FreeNode(newSize, currNode->next);
    }
    else
    {
        allocSize += newSize;
        newNode = currNode->next;
    }

    if (prevNode == nullptr)
    {
        pHead = newNode;
    }
    else
    {
        prevNode->next = newNode;
    }

    // Place allocation header in front of allocated memory section
    AllocHeader *header = reinterpret_cast<AllocHeader*>(alignedAddress - sizeof(AllocHeader));
    header->size = allocSize;
    header->adjustment = adjustment;

    mUsedMemory += header->adjustment + sizeof(AllocHeader) + header->size;
    mMaxUsedMemory = std::max(mMaxUsedMemory, mUsedMemory);

    return reinterpret_cast<void*>(alignedAddress);
}


void FreeListAllocator::Free(void *ptr) {

    assert(ptr != nullptr);

    // lock mutex
    const std::lock_guard<std::mutex> lock( mMutex);

    // Start address and size of freed memory section
    UPtr freeAddress = reinterpret_cast<UPtr>(ptr);
    AllocHeader *header = reinterpret_cast<AllocHeader*>( freeAddress - sizeof(AllocHeader) );
    freeAddress -= header->adjustment + sizeof(AllocHeader);
    Size freeSize = header->adjustment + sizeof(AllocHeader) + header->size;

    mUsedMemory -= freeSize;

    // Find adjacent nodes
    FreeNode *nextNode = pHead, *prevNode = nullptr;
    while (nextNode && nextNode->address < freeAddress)
    {
        prevNode = nextNode;
        nextNode = nextNode->next;
    }

    // Combine the freed memory section with the adjacent nodes if necessary.
    if (prevNode && prevNode->address + prevNode->size == freeAddress)
    {
        freeAddress = prevNode->address;
        freeSize += prevNode->size;
    }
    if (nextNode && nextNode->address == freeAddress + freeSize)
    {
        freeSize += nextNode->size;
        nextNode = nextNode->next;
    }
    
    // Create a new node for the freed section, this may override prevNode, but all pointers are still valid.
    FreeNode *newNode = new (reinterpret_cast<void*>(freeAddress)) FreeNode(freeSize, nextNode);
    
    if (prevNode == nullptr)
    {
        pHead = newNode;
        return;
    }
    if (prevNode != newNode)
    {
        prevNode->next = newNode;
    }
}


void FreeListAllocator::Clear() {

    // lock mutex
    const std::lock_guard<std::mutex> lock( mMutex);
    
    pHead = new (pBase) FreeNode{mTotalMemory};
    mUsedMemory = 0;
}

    
} // Atuin
