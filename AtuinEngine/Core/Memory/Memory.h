
#pragma once


#include "MemoryManager.h"
#include "Core/Util/Types.h"


namespace Atuin {


/* @brief Interface to easily use MemoryMananger throughout the code base.
 *        Will default to using malloc, free, new, delete if MemoryManager is  not initialized yet.
 */
class Memory {

    friend class MemoryManager;

public:

    Memory() : pMemoryManager {sMemoryManager} {}

    void* Allocate(Size size, U8 alignment);
    void  Free(void *ptr);

    template<typename T, typename... Args>
    T* New(Args&&... args);

    template<typename T>
    T* NewArray(Size size);

    template<typename T>
    void Delete(T *obj);

    template<typename T>
    void DeleteArray(T *arr, Size size);


private:

    static MemoryManager* sMemoryManager;
    
    MemoryManager* pMemoryManager;
};


template<typename T, typename... Args>
T* Memory::New(Args&&... args) {

    if (pMemoryManager != nullptr) 
    {
        return pMemoryManager->New<T>(std::forward<Args>(args)...);
    }

    return new T(std::forward<Args>(args)...);
}


template<typename T>
T* Memory::NewArray(Size size) {

    if (pMemoryManager != nullptr) 
    {
        return pMemoryManager->NewArray<T>(size);
    }

    return new T[size];
}


template<typename T>
void Memory::Delete(T *obj) {

    if (pMemoryManager != nullptr) 
    {
        pMemoryManager->Delete(obj);
    }
    else
    {
        delete obj;
    }
}


template<typename T>
void Memory::DeleteArray(T *arr, Size size) {

    if (pMemoryManager != nullptr) 
    {
        pMemoryManager->DeleteArray(arr, size);
    }
    else
    {
        delete[] arr;
    }
}

    
} // Atuin
