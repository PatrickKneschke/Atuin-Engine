
#pragma once


#include "Core/Util/Types.h"
#include "Core/Memory/MemoryManager.h"
#include "Core/Util/Math.h"
#include "Core/Util/StringFormat.h"

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <utility>


namespace Atuin {


class MemoryManager;

template<typename T>
class Array {

public:

    static MemoryManager *pMemory;


    Array();
    Array(Size capacity);
    Array(Size capacity, const T &value);
    Array(const std::initializer_list<T> &list);

    Array(const Array &other);
    Array(Array &&other);

    Array& operator= (const Array &rhs);
    Array& operator= (Array &&rhs);

    ~Array();


    bool IsEmpty() { return mSize == 0; }
    Size GetSize() { return mSize; }
    Size GetCapacity() { return mCapacity; }

    void Clear();
    void Reserve(Size capacity);
    void Resize(Size newSize);
    void Resize(Size newSize, const T &value);
    void PushBack(const T &value);
    void PushBack(T &&value);
    void PopBack();
    template<typename... Args>
    T&   EmplaceBack(Args&&... args);

    T*       Data() { return pData; }
    const T* Data() const { return pData; }
    T&       Front();
    const T& Front() const;
    T&       Back();
    const T& Back() const;
    T&       operator[](Size idx);
    const T& operator[](Size idx) const;


private:

    void Allocate(Size capacity);
    void Free();

    Size mSize;
    Size mCapacity;

    T* pData;
};


template<typename T>
MemoryManager* Array<T>::pMemory = nullptr;


template<typename T>
void Array<T>::Allocate(Size capacity) {

    if (mCapacity == 0)
    {
        return;
    }

    if (pMemory == nullptr)
    {
        pData = static_cast<T*>( malloc(mCapacity * sizeof(T)) );
    }
    else
    {
        pData = static_cast<T*>( pMemory->Allocate(mCapacity * sizeof(T), alignof(T)) );
    }
    mCapacity = capacity;
}


template<typename T>
void Array<T>::Free() {

    if (pData == nullptr)
    {
        return;
    }    

    if (pMemory == nullptr)
    {
        free(static_cast<void*>(pData));
    }
    else
    {
        pMemory->Free(static_cast<void*>(pData));
    }

    mCapacity = 0;
}


template<typename T>
Array<T>::Array() : mSize {0}, mCapacity {0}, pData {nullptr} {}


template<typename T>
Array<T>::Array(Size capacity) : mSize {0}, mCapacity {capacity}, pData {nullptr} {

    Allocate(mCapacity);
}


template<typename T>
Array<T>::Array(Size capacity, const T &value) : mSize {capacity}, mCapacity {capacity}, pData {nullptr} {

    Allocate(mCapacity);
    std::fill_n(pData, mCapacity, value);
}


template<typename T>
Array<T>::Array(const std::initializer_list<T> &list) : mSize {0}, mCapacity {list.size()}, pData {nullptr} {

    Allocate(mCapacity);
    for(auto it = list.begin(); it != list.end(); it++)
    {
        pData[mSize] = *it;
        ++mSize; 
    }
}


template<typename T>
Array<T>::Array(const Array &other) : mSize {other.mSize}, mCapacity {other.mCapacity} {

    Allocate(mCapacity);
    for (Size i = 0; i < mSize; i++)
    {
        pData[i] = other[i];
    }
}


template<typename T>
Array<T>::Array(Array &&other) : mSize {other.mSize}, mCapacity {other.mCapacity}, pData {other.pData} {

    other.mSize = 0;
    other.mCapacity = 0;
    other.pData = nullptr;
}


template<typename T>
Array<T>& Array<T>::operator= (const Array &rhs) {

    if (this != &rhs)
    {
        Clear();
        Free();
        Allocate(rhs.mCapacity);
        for(Size i = 0; i < mSize; i++)
        {
            PushBack(rhs[i]);
        }
    }    

    return *this;
}


template<typename T>
Array<T>& Array<T>::operator= (Array &&rhs) {

    if (this != &rhs)
    {
        Clear();
        Free();

        mSize = rhs.mSize;
        mCapacity = rhs.mCapacity;
        pData = rhs.pData;

        rhs.mSize = 0;
        rhs.mCapacity = 0;
        rhs.pData = nullptr;      
    }

    return *this;
}


template<typename T>
Array<T>::~Array() {

    Clear();
    Free();
    pData = nullptr;
}


template<typename T>
void Array<T>::Clear() {

    for (Size i = 0; i < mSize; i++)
    {
        pData[i].~T();
    }

    mSize = 0;
}


template<typename T>
void Array<T>::Reserve(Size capacity) {

    if (capacity <= mCapacity)
    {
        return;
    }

    Array<T> temp(capacity);
    for (Size i = 0; i < mSize; i++)
    {
        temp.PushBack(pData[i]);
    }
    
    *this = std::move(temp);
}


template<typename T>
void Array<T>::Resize(Size newSize) {

    for ( Size i = newSize; i < mSize; i++)
    {
        pData[i].~T();
    }
    
    Reserve(newSize);
    if (newSize > mSize)
    {
        std::fill_n(pData+mSize, newSize-mSize, T());
    }

    mSize = newSize;    
}


template<typename T>
void Array<T>::Resize(Size newSize, const T &value) {

    for ( Size i = newSize; i < mSize; i++)
    {
        pData[i].~T();
    }
    
    Reserve(newSize);
    std::fill_n(pData+mSize, newSize-mSize, value);

    mSize = newSize;    
}


template<typename T>
void Array<T>::PushBack(const T &value) {

    if (mSize == mCapacity)
    {
        Reserve( Math::NextPowerOfTwo(mCapacity) );
    }

    pData[mSize++] = value;
}


template<typename T>
void Array<T>::PushBack(T &&value) {

    if (mSize == mCapacity)
    {
        Reserve( Math::NextPowerOfTwo(mCapacity) );
    }

    pData[mSize++] = std::move(value);
}


template<typename T>
void Array<T>::PopBack() {

    if (mSize == 0)
    {
        return;
    }

    pData[--mSize].~T();
}


template<typename T>
template<typename... Args>
T& Array<T>::EmplaceBack(Args&&... args) {

    if (mSize == mCapacity)
    {
        Reserve( Math::NextPowerOfTwo(mCapacity) );
    }

    return *(new ( reinterpret_cast<void*>(pData + mSize++) ) T(std::forward<Args>(args)...));
}


template<typename T>
T& Array<T>::Front() {

    if (mSize == 0)
    {
        throw std::out_of_range("Array::Front called on empty array.");
    }    

    return pData[0];
}


template<typename T>
const T& Array<T>::Front() const {

    if (mSize == 0)
    {
        throw std::out_of_range("Array::Front called on empty array.");
    }    

    return pData[0];
}


template<typename T>
T& Array<T>::Back() {

    if (mSize == 0)
    {
        throw std::out_of_range("Array::Back called on empty array.");
    }    

    return pData[mSize-1];
}


template<typename T>
const T& Array<T>::Back() const {

    if (mSize == 0)
    {
        throw std::out_of_range("Array::Back called on empty array.");
    }    

    return pData[mSize-1];
}


template<typename T>
T& Array<T>::operator[](Size idx) {

    if (idx >= mSize)
    {
        throw std::out_of_range( FormatStr("Array::operator[] index  %d  out of range [0 ... %d].", idx, mSize-1) );
    }    

    return pData[idx];
}


template<typename T>
const T& Array<T>::operator[](Size idx) const {

    if (idx >= mSize)
    {
        throw std::out_of_range( FormatStr("Array::operator[] index  %d  out of range [0 ... %d].", idx, mSize-1) );
    }    

    return pData[idx];
}


} // Atuin
