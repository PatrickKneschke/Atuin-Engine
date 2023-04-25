
#pragma once


#include "Core/Config/CVar.h"
#include "Core/Util/Types.h"

#include <memory>


namespace Atuin {


class EngineLoop;
class FreeListAllocator;

class MemoryManager {


public:

    MemoryManager(EngineLoop *engine);
    ~MemoryManager() = default;


    void* Allocate(Size size, U8 alignment);
    void  Free(void *ptr);

    template<typename T, typename... Args>
    T* New(const Args&... args);

    template<typename T>
    T* NewArray(Size size);

    template<typename T>
    void Delete(T *obj);

    template<typename T>
    void DeleteArray(T *arr, Size size);


private:

    static CVar<Size>* pHeapMemorySize;

    std::unique_ptr<FreeListAllocator> pHeapMemory;

    // TODO ? add entity pool
    // TODO ? add pool for colliders
    // TODO ? add pools for graphic assets and sound clips
    // -> everything updated together stays together in memory for cache friendlyness ?

    EngineLoop* pEngine;
};


template<typename T, typename... Args>
T* MemoryManager::New(const Args&... args) {

    void *mem = Allocate(sizeof(T), alignof(T));
    return new (mem) T(args...)
}


template<typename T>
T* MemoryManager::NewArray(Size size) {

    T *mem = static_cast<T*>( Allocate(size * sizeof(T), alignof(T)) );
    for (Size i = 0; i < size; i++)
    {    
        new (mem + i) T();
    }

    return mem;
}


template<typename T>
void MemoryManager::Delete(T *obj) {

    obj->~T();
    Free(static_cast<void*>(obj));
}


template<typename T>
void MemoryManager::DeleteArray(T *arr, Size size) {

    for(Size i=0; i<size; i++)
    {
        arr[i].~T();
    }

    Free(static_cast<void*>(arr));
}

    
} //  Atuin
