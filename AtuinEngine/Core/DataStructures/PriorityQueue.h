
#pragma once


#include "Array.h"
#include "Core/Util/Types.h"
#include "Core/Util/Math.h"
#include "Core/Debug/Log.h"

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <utility>


namespace Atuin {


template<typename T, class Compare = Math::Less<T>>
class PriorityQueue {

public:

    PriorityQueue();
    PriorityQueue(Size capacity);
    PriorityQueue(const std::initializer_list<T> &list);
    PriorityQueue(const Array<T> &array);
    PriorityQueue(Array<T> &&array);

    PriorityQueue(const PriorityQueue &other);
    PriorityQueue(PriorityQueue &&other);

    PriorityQueue& operator= (const PriorityQueue &other);
    PriorityQueue& operator= (PriorityQueue &&other);
    PriorityQueue& operator= (const std::initializer_list<T> &list);

    ~PriorityQueue() = default;

    bool IsEmpty() { return mData.GetSize() == 0; }
    Size GetSize() { return mData.GetSize(); }

    void Clear() { mData.Clear(); }
    void Push(const T &value);
    void Push(T &&value);
    void Pop();
    template<typename... Args>
    void Emplace(Args&&... args);

    T&       Top();
    const T& Top() const;


private:

    void MakeHeap(Size rootIdx);
    Size Parent(Size idx);
    Size Left(Size idx);
    Size Right(Size idx);
    void PushToTop(Size idx);


    Array<T> mData;
    Compare mCompare;

    Log mLog;
};


template<typename T,  class Compare>
PriorityQueue<T,Compare>::PriorityQueue() : mData(), mLog() {}


template<typename T, class Compare>
PriorityQueue<T,Compare>::PriorityQueue(Size capacity) : mData(capacity), mLog() {}


template<typename T, class Compare>
PriorityQueue<T,Compare>::PriorityQueue(const std::initializer_list<T> &list) : mData(list), mLog() {

    MakeHeap(0);
}


template<typename T, class Compare>
PriorityQueue<T,Compare>::PriorityQueue(const Array<T> &array) : mData(array), mLog() {

    MakeHeap(0);
}


template<typename T, class Compare>
PriorityQueue<T,Compare>::PriorityQueue(Array<T> &&array) : mData {std::move(array)}, mLog() {

    MakeHeap(0);
}


template<typename T, class Compare>
PriorityQueue<T,Compare>::PriorityQueue(const PriorityQueue<T,Compare> &other) : mData {other.mData}, mLog() {}


template<typename T, class Compare>
PriorityQueue<T,Compare>::PriorityQueue(PriorityQueue<T,Compare> &&other) : mData {std::move(other.mData)}, mLog() {}


template<typename T, class Compare>
PriorityQueue<T,Compare>& PriorityQueue<T,Compare>::operator= (const PriorityQueue<T, Compare> &other) {

    mData = other.mData;
}


template<typename T, class Compare>
PriorityQueue<T,Compare>& PriorityQueue<T,Compare>::operator= (PriorityQueue<T, Compare> &&other) {

    mData = std::move(other.Data);
}


template<typename T, class Compare>
PriorityQueue<T,Compare>& PriorityQueue<T,Compare>::operator= (const std::initializer_list<T> &list) {

    mData = list;
}


template<typename T, class Compare>
void PriorityQueue<T,Compare>::Push(const T &value) {

    mData.PushBack(value);
    PushToTop( mData.GetSize() - 1 );
}
    
    
template<typename T, class Compare>
void PriorityQueue<T,Compare>::Push(T &&value) {

    mData.PushBack( std::move(value) );
    PushToTop( mData.GetSize() - 1 );
}
    
    
template<typename T, class Compare>
void PriorityQueue<T,Compare>::Pop() {

    std::swap( mData.Front(), mData.Back() );
    mData.PopBack();
    MakeHeap(0);
}


template<typename T, class Compare>
template<typename... Args>
void PriorityQueue<T,Compare>::Emplace(Args&&... args) {

    mData.EmplaceBack( std::forward<Args>(args)... );
    PushToTop( mData.GetSize() - 1 );
}


template<typename T, class Compare>
T& PriorityQueue<T,Compare>::Top() {

    if ( mData.IsEmpty() )
    {
        mLog.Error( LogChannel::GENERAL, "PriorityQueue::Top called on empty queue." );
    }

    return mData[0];
}


template<typename T, class Compare>
const T& PriorityQueue<T,Compare>::Top() const {

    if ( mData.IsEmpty() )
    {
        mLog.Error( LogChannel::GENERAL, "PriorityQueue::Top called on empty queue." );
    }

    return mData[0];
}


template<typename T, class Compare>
void PriorityQueue<T,Compare>::MakeHeap(Size rootIdx) {

    Size left = Left(rootIdx);
    Size right = Right(rootIdx);
    Size minIdx = rootIdx;

    if (left < mData.GetSize() && mCompare(mData[left], mData[minIdx])) 
    {
        minIdx = left;
    }
    if (right < mData.GetSize() && mCompare(mData[right], mData[minIdx])) 
    {
        minIdx = right;
    }
    if (minIdx != rootIdx)
    {
        std::swap(mData[rootIdx], mData[minIdx]);
        MakeHeap(minIdx);
    }
}


template<typename T, class Compare>
Size PriorityQueue<T,Compare>::Parent(Size idx) {

    return (idx - 1) / 2;
}


template<typename T, class Compare>
Size PriorityQueue<T,Compare>::Left(Size idx) {

    return 2 * idx + 1;
}


template<typename T, class Compare>
Size PriorityQueue<T,Compare>::Right(Size idx) {

    return 2 * idx + 2;
}


template<typename T, class Compare>
void PriorityQueue<T,Compare>::PushToTop(Size idx) {

    Size parent = Parent(idx);
    while (idx>0 && mCompare( mData[idx], mData[parent]))
    {
        std::swap(mData[idx], mData[parent]);
        idx = parent;
        parent = Parent(idx);
    }
}


} // Atuin
