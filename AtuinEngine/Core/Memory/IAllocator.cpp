
#include "IAllocator.h"


namespace Atuin {


IAllocator::IAllocator(Size totalMemory, IAllocator *parent) :
    mTotalMemory {totalMemory},
    mUsedMemory {0},
    mMaxUsedMemory {0},
    pParent {parent} {

    assert(mTotalMemory > 0);

    if(pParent == nullptr)
    {
        pBase = malloc(mTotalMemory);
    }
    else
    {
        pBase = pParent->Allocate(mTotalMemory, sizeof(max_align_t));
    }
}


IAllocator::~IAllocator() {

    if(pParent == nullptr)
    {
        free(pBase);
    }
    else
    {
        pParent->Free(pBase);
    }
}


Size IAllocator::GetAlignmentAdjustment(PtrInt address, Size alignment) {

    // alignment must be positive ad power of two
    assert( alignment > 0 ); 
    assert( (alignment & (alignment - 1)) == 0 );

    Size adjustment = alignment - address & (alignment - 1);
    // If address is already properly aligned then adjustment = 0
    adjustment &= alignment - 1;

    return adjustment;
}

    
} // Atuin
