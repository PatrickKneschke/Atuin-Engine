
#pragma once


#include "IAllocator.h"


namespace Atuin {


class StackAllocator : public IAllocator {

public:

    StackAllocator() = delete;
    StackAllocator(Size totalMemory, IAllocator *parent);
    ~StackAllocator();


    void* Allocate(Size size, U8 alignment) override;
    void  Free(void *ptr) override;
    void  Clear() override;


private:

    UPtr mBaseAddress;
    UPtr mTopAddress;    

};
    
    
} // Atuin
