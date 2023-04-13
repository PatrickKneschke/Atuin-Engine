
#pragma once


#include "IAllocator.h"


namespace Atuin {


class PoolAllocator : public IAllocator {

    struct PoolNode {

        PoolNode *next;
    };

public:
    
    PoolAllocator() = delete;
    PoolAllocator(Size numChunks, Size chunkSize, IAllocator *parent);
    ~PoolAllocator();

    void* Allocate();
    void* Allocate(Size size, U8 alignment) override;
    void  Free(void *ptr) override;
    void  Clear() override;


private:

    void CreatePool();

    Size mNumChunks;
    Size mChunkSize;

    PoolNode *pHead;
};


} // Atuin
