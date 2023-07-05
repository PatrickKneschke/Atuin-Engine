
#pragma once


#include "Core/Util/Types.h"
#include "Core/Memory/MemoryManager.h"
#include "Core/Util/Math.h"
#include "Core/Util/StringFormat.h"

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <utility>

#include <iostream>


namespace Atuin {


class MemoryManager;

template<typename T>
class Array {

public:

    class iterator {

        public:

            explicit iterator(T *ptr_) : ptr {ptr_} {}

            iterator operator+(int rhs) {
                
                iterator it = *this;
                it.ptr += rhs;

                return it;
            }

            iterator operator-(int rhs) {
                
                iterator it = *this;
                it.ptr -= rhs;
                
                return it;
            }

            int operator-(iterator rhs) { 
                
                return reinterpret_cast<UPtr>(ptr) - reinterpret_cast<UPtr>(rhs.ptr); 
            }
            
            iterator operator++() {
            
                ++ptr;
                return *this;
            }

            iterator operator++(int) {
            
                iterator it = *this;
                ++ptr;

                return it;
            }

            iterator operator--() {

                --ptr;

                return *this;
            }

            iterator operator--(int) {

                iterator it = *this;
                --ptr;

                return it;
            }

            T&  operator*() { return *ptr; }
            T* operator->() { return ptr; }
            bool operator==(const iterator &rhs) { return ptr == rhs.ptr; }
            bool operator!=(const iterator &rhs) { return ptr != rhs.ptr; }

        private:
            T *ptr;
    };

    class const_iterator {

        public:

            explicit const_iterator(T *ptr_) : ptr {ptr_} {}

            const_iterator operator+(int rhs) {
                
                const_iterator it = *this;
                it.ptr += rhs;

                return it;
            }

            const_iterator operator-(int rhs) {
                
                const_iterator it = *this;
                it.ptr -= rhs;
                
                return it;
            }

            int operator-(const_iterator rhs) { 
                
                return reinterpret_cast<UPtr>(ptr) - reinterpret_cast<UPtr>(rhs.ptr); 
            }
            
            const_iterator operator++() {
            
                ++ptr;
                return *this;
            }

            const_iterator operator++(int) {
            
                const_iterator it = *this;
                ++ptr;

                return it;
            }

            const_iterator operator--() {

                --ptr;

                return *this;
            }

            const_iterator operator--(int) {

                const_iterator it = *this;
                --ptr;

                return it;
            }

            const T& operator*() { return *ptr; }
            const T* operator->() { return ptr; }
            bool operator==(const const_iterator &rhs) { return ptr == rhs.ptr; }
            bool operator!=(const const_iterator &rhs) { return ptr != rhs.ptr; }

        private:
            T *ptr;
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;


public:

    Array(MemoryManager *memory = nullptr);
    Array(Size capacity, MemoryManager *memory = nullptr);
    Array(Size capacity, const T &value, MemoryManager *memory = nullptr);
    Array(const std::initializer_list<T> &list, MemoryManager *memory = nullptr);

    Array(const Array &other, MemoryManager *memory = nullptr);
    Array(Array &&other) noexcept;

    Array& operator= (const Array &rhs);
    Array& operator= (Array &&rhs) noexcept;
    Array& operator= (const std::initializer_list<T> &list);

    ~Array();


    bool IsEmpty() const { return mSize == 0; }
    Size GetSize() const { return mSize; }
    Size GetCapacity() const { return mCapacity; }

    void Clear();
    void Reserve(Size capacity);
    void Resize(Size newSize);
    void Resize(Size newSize, const T &value);
    void PushBack(const T &value);
    void PushBack(T &&value);
    void PopBack();
    template<typename... Args>
    T&   EmplaceBack(Args&&... args);
    void Insert(Size pos, const T &value);
    void Insert(Size pos, T &&value);
    void Erase(Size pos);

    T*       Data() { return pData; }
    const T* Data() const { return pData; }
    T&       Front();
    const T& Front() const;
    T&       Back();
    const T& Back() const;
    T&       operator[](Size idx);
    const T& operator[](Size idx) const;

    iterator Begin() { return iterator(pData); }
    const_iterator Cbegin() const { return const_iterator(pData); }
    iterator End() { return iterator(pData + mSize); }
    const_iterator Cend() const { return const_iterator(pData + mSize); }
    reverse_iterator Rbegin() { return reverse_iterator(pData + mSize - 1); }
    const_iterator Crbegin() const { return const_reverse_iterator(pData + mSize - 1); }
    iterator Rend() { return reverse_iterator(pData - 1); }
    const_iterator Crend() const { return const_iterator(pData - 1); }

    bool operator== (const Array &rhs) const;
    bool operator!= (const Array &rhs) const;


private:

    void Allocate(Size capacity);
    void Free();

    MemoryManager *pMemory;

    Size mSize;
    Size mCapacity;

    T* pData;
};


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
    pData = nullptr;
}


template<typename T>
Array<T>::Array(MemoryManager *memory) : pMemory {memory}, mSize {0}, mCapacity {0}, pData {nullptr} {}


template<typename T>
Array<T>::Array(Size capacity, MemoryManager *memory) : pMemory {memory}, mSize {0}, mCapacity {capacity}, pData {nullptr} {

    Allocate(mCapacity);
}


template<typename T>
Array<T>::Array(Size capacity, const T &value, MemoryManager *memory) : pMemory {memory}, mSize {capacity}, mCapacity {capacity}, pData {nullptr} {

    Allocate(mCapacity);
    std::fill_n(pData, mCapacity, value);
}


template<typename T>
Array<T>::Array(const std::initializer_list<T> &list, MemoryManager *memory) : pMemory {memory}, mSize {0}, mCapacity {list.size()}, pData {nullptr} {

    Allocate(mCapacity);
    for(auto it = list.begin(); it != list.end(); it++)
    {
        pData[mSize] = *it;
        ++mSize; 
    }
}


template<typename T>
Array<T>::Array(const Array &other, MemoryManager *memory) : pMemory {memory}, mSize {other.mSize}, mCapacity {other.mCapacity} {

    Allocate(mCapacity);
    for (Size i = 0; i < mSize; i++)
    {
        pData[i] = other[i];
    }
}


template<typename T>
Array<T>::Array(Array &&other) noexcept : pMemory {other.pMemory}, mSize {other.mSize}, mCapacity {other.mCapacity}, pData {other.pData} {

    other.pMemory = nullptr;
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
Array<T>& Array<T>::operator= (Array &&rhs) noexcept {

    if (this != &rhs)
    {
        Clear();
        Free();

        pMemory = rhs.pMemory;
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
Array<T>& Array<T>::operator= (const std::initializer_list<T> &list) {

    Clear();
    Free();
    Allocate(list.size());
    for(auto it = list.begin(); it != list.end(); it++)
    {
        PushBack(*it);
    }

    return *this;
}


template<typename T>
Array<T>::~Array() {

    Clear();
    Free();
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

    if constexpr (!std::is_copy_assignable_v<T> && !std::is_move_assignable_v<T>)
    {
        throw std::logic_error("Cannot reserve more space for array of non-copyable, non-movable type.");
    }
    else
    {
        Array<T> temp(capacity);
        for (Size i = 0; i < mSize; i++)
        {
            std::cout << "move old " << i << '\n';

            temp.PushBack( std::move(pData[i]) );
        }
        *this = std::move(temp);
    }
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
    if (newSize > mSize)
    {
        std::fill_n(pData+mSize, newSize-mSize, value);
    }
    
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
    
    std::cout << "move push\n";

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
void Array<T>::Insert(Size pos, const T &value) {

    if (pos > mSize)
    {
        return;
    }

    PushBack(value);
    for (Size i = mSize-1; i > pos; i--)
    {
        std::swap(pData[i], pData[i-1]);
    }    
}


template<typename T>
void Array<T>::Insert(Size pos, T &&value) {

    if (pos > mSize)
    {
        return;
    }

    PushBack(std::move(value));
    for (Size i = mSize-1; i > pos; i--)
    {
        std::swap(pData[i], pData[i-1]);
    }
}


template<typename T>
void Array<T>::Erase(Size pos) {

    if (pos >= mSize)
    {
        return;
    }

    for (Size i = pos; i < mSize-1; i++)
    {
        std::swap(pData[i], pData[i+1]);
    }
    PopBack();
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


template<typename T>
bool Array<T>::operator== (const Array &rhs) const {

    if (mSize != rhs.GetSize())
    {
        return false;
    }    

    for (Size i = 0; i < mSize; i++)
    {
        if (pData[i] != rhs[i])
        {
            return false;
        }        
    }    

    return true;
}
    

template<typename T>    
bool Array<T>::operator!= (const Array &rhs) const {

    return !( *this == rhs );
}



template<typename T>
Array<T>::iterator begin(Array<T> &arr) { return arr.Begin(); }

template<typename T>
Array<T>::const_iterator begin(const Array<T> &arr) { return arr.Cbegin(); }

template<typename T>
Array<T>::iterator end(Array<T> &arr) { return arr.End(); }

template<typename T>
Array<T>::const_iterator end(const Array<T> &arr) { return arr.Cend(); }

template<typename T>
Array<T>::iterator rbegin(Array<T> &arr) { return arr.Rbegin(); }

template<typename T>
Array<T>::iterator rbegin(const Array<T> &arr) { return arr.Crbegin(); }

template<typename T>
Array<T>::iterator rend(Array<T> &arr) { return arr.Rend(); }

template<typename T>
Array<T>::iterator rend(const Array<T> &arr) { return arr.Crend(); }


} // Atuin
