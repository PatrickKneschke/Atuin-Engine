
#pragma once 


#include "IAllocator.h"


namespace Atuin {


class FreeListAllocator : public IAllocator{


    struct FreeNode {

        Size size;
        UPtr address;
        FreeNode *next;

        FreeNode() : size {0}, address {0}, next {nullptr} {}
        FreeNode(const size_t size_, FreeNode *next_ = nullptr) : size {size_}, next {next_} {

            address = reinterpret_cast<UPtr>(this);
        }    
    };


    struct AllocHeader {

        Size size;
        Size adjustment;
    };


public:

    FreeListAllocator() = delete;
    FreeListAllocator(Size totalMemory, IAllocator *parent = nullptr);
    ~FreeListAllocator();

    
    void* Allocate(Size size, U8 alignment) override;
    void Free(void *ptr) override;
    void Clear() override;


private:

    FreeNode *pHead;
};

    
} // Atuin
