
#include "IAllocator.h"
#include "Core/Util/Math.h"

#include <cstdlib>


namespace Atuin {


IAllocator::IAllocator(Size totalMemory, IAllocator *parent) :
    mTotalMemory {totalMemory},
    mUsedMemory {0},
    mMaxUsedMemory {0},
    pParent {parent}
{
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


Size IAllocator::GetAlignmentAdjustment(UPtr address, Size alignment) {

    assert( alignment > 0 ); 
    assert( Math::IsPowerOfTwo(alignment) );

    Size adjustment = alignment - Math::ModuloPowerOfTwo(address, alignment);

    return Math::ModuloPowerOfTwo(adjustment, alignment);
}

    
} // Atuin
