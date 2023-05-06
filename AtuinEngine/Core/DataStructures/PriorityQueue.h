
#pragma once


#include "Array.h"
#include "Core/Util/Types.h"
#include "Core/Util/Math.h"

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <utility>


namespace Atuin {


template<typename T, template<typename> class Compare>
class PriorityQueue {

public:

    PriorityQueue(const Compare<T> &compare = Math::Less<T>{});
    PriorityQueue(Size capacity, const Compare<T> &compare = Math::Less<T>{});
    PriorityQueue(const std::initializer_list<T> &list, const Compare<T> &compare = Math::Less<T>{});
    PriorityQueue(const Array<T> &array, const Compare<T> &compare = Math::Less<T>{});
    PriorityQueue(Array<T> &&array, const Compare<T> &compare = Math::Less<T>{});

    PriorityQueue(const PriorityQueue &other);
    PriorityQueue(PriorityQueue &&other);

    PriorityQueue& operator= (const PriorityQueue &other);
    PriorityQueue& operator= (PriorityQueue &&other);

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
    Compare<T> mCompare;
};


template<typename T, template<typename> class Compare>
PriorityQueue<T,Compare>::PriorityQueue(const Compare<T> &compare) : mData(), mCompare {compare} {}


template<typename T, template<typename> class Compare>
PriorityQueue<T,Compare>::PriorityQueue(Size capacity, const Compare<T> &compare) : mData(capacity), mCompare {compare} {}


template<typename T, template<typename> class Compare>
PriorityQueue<T,Compare>::PriorityQueue(const std::initializer_list<T> &list, const Compare<T> &compare) : mData {list}, mCompare {compare} {

    MakeHeap(0);
}


template<typename T, template<typename> class Compare>
PriorityQueue<T,Compare>::PriorityQueue(const Array<T> &array, const Compare<T> &compare) : mData {array}, mCompare {compare} {

    MakeHeap(0);
}


template<typename T, template<typename> class Compare>
PriorityQueue<T,Compare>::PriorityQueue(Array<T> &&array, const Compare<T> &compare) : mData {std::move(array)}, mCompare {compare} {

    MakeHeap(0);
}


template<typename T, template<typename> class Compare>
PriorityQueue<T,Compare>::PriorityQueue(const PriorityQueue<T,Compare> &other) : mData {other.mData}, mCompare {other.mCompare} {}


template<typename T, template<typename> class Compare>
PriorityQueue<T,Compare>::PriorityQueue(PriorityQueue<T,Compare> &&other) : mData {std::move(other.mData)}, mCompare {other.mCompare} {}


template<typename T, template<typename> class Compare>
PriorityQueue<T,Compare>& PriorityQueue<T,Compare>::operator= (const PriorityQueue<T, Compare> &other) {

    mData = other.mData;
    mCompare = other.mCompare;
}


template<typename T, template<typename> class Compare>
PriorityQueue<T,Compare>& PriorityQueue<T,Compare>::operator= (PriorityQueue<T, Compare> &&other) {

    mData = std::move(other.Data);
    mCompare = other.mCompare;
}


template<typename T, template<typename> class Compare>
void PriorityQueue<T,Compare>::Push(const T &value) {

    mData.PushBack(value);
    PushToTop( mData.GetSize() - 1 );
}
    
    
template<typename T, template<typename> class Compare>
void PriorityQueue<T,Compare>::Push(T &&value) {

    mData.PushBack( std::move(value) );
    PushToTop( mData.GetSize() - 1 );
}
    
    
template<typename T, template<typename> class Compare>
void PriorityQueue<T,Compare>::Pop() {

    std::swap( mData.Front(), mData.Back() );
    mData.PopBack();
    MakeHeap(0);
}


template<typename T, template<typename> class Compare>
template<typename... Args>
void PriorityQueue<T,Compare>::Emplace(Args&&... args) {

    mData.EmplaceBack( std::forward<Args>(args)... );
}


template<typename T, template<typename> class Compare>
T& PriorityQueue<T,Compare>::Top() {

    if ( mData.IsEmpty() )
    {
        throw std::out_of_range("PriorityQueue::Top called on empty queue.");
    }

    return mData[0];
}


template<typename T, template<typename> class Compare>
const T& PriorityQueue<T,Compare>::Top() const {

    if ( mData.IsEmpty() )
    {
        throw std::out_of_range("PriorityQueue::Top called on empty queue.");
    }

    return mData[0];
}


template<typename T, template<typename> class Compare>
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


template<typename T, template<typename> class Compare>
Size PriorityQueue<T,Compare>::Parent(Size idx) {

    return (idx - 1) / 2;
}


template<typename T, template<typename> class Compare>
Size PriorityQueue<T,Compare>::Left(Size idx) {

    return 2 * idx + 1;
}


template<typename T, template<typename> class Compare>
Size PriorityQueue<T,Compare>::Right(Size idx) {

    return 2 * idx + 2;
}


template<typename T, template<typename> class Compare>
void PriorityQueue<T,Compare>::PushToTop(Size idx) {

    Size parent = Parent(idx);
    while (idx>0 && mCompare(idx, parent))
    {
        std::swap(mData[idx], mData[parent]);
        idx = parent;
        parent = Parent(idx);
    }
}


} // Atuin
