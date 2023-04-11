
#pragma once


#include "Core/Util/Types.h"


namespace Atuin {
    

class IAllocator {

public:

    ~IAllocator();

    Size  totalMemory()   const { return mTotalMemory;}
    Size  usedMemory()    const { return mUsedMemory;}
    Size  maxUsedMemory() const { return mMaxUsedMemory;}

    virtual void* Allocate(Size size, U8 alignment = 1) = 0;
    virtual void  Free(void *ptr) = 0;
    virtual void  Clear() = 0;

    // TODO pass args by reference ?
    template<typename T, typename... Args>
    T* New(Args... args);

    template<typename T>
    T* NewArray(const Size size);

    template<typename T>
    void Delete(T *obj);

    template<typename T>
    void DeleteArr(T *arr, Size size);


protected:

    IAllocator() = delete;
    IAllocator(Size totalMemory, IAllocator *parent = nullptr);


    size_t GetAlignmentAdjustment(PtrInt address, Size alignment);


    // Poiner to a parent allocator, nullptr by default
    IAllocator *pParent;

    // Pointer to the beginning of the allocated memory
    void* pBase;

    Size mTotalMemory;
    Size mUsedMemory;
    Size mMaxUsedMemory;
};


template<typename T, typename... Args>
T* IAllocator::New(Args... args) {

    void *mem = Allocate(sizeof(T), alignof(T));
    return new (mem) T(args);
}


template<typename T>
T* IAllocator::NewArray(Size size) {

    // TODO assert size > 0

    T *mem = static_cast<T*>( Allocate(size * sizeof(T), alignof(T)) );
    for (Size i = 0; i < size; i++)
    {    
        new (mem + i) T();
    }

    return mem;
}


template<typename T>
void IAllocator::Delete(T *obj) {

    obj->~T();
    Free(static_cast<void*>(obj));
}


template<typename T>
void IAllocator::DeleteArr(T *arr, Size size) {

    // TODO assert size > 0

    for(Size i=0; i<size; i++)
    {
        arr[i]~T();
    }

    Free(static_cast<void*>(arr));
}


} // Atuin
