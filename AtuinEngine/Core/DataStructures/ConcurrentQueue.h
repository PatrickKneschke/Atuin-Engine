
#pragma once


#include "Array.h"
#include "Core/Util/Math.h"

#include <atomic>


namespace Atuin {


/* @brief Single producer, multiple consumer queue. Push/Pop are LIFO local to one thread, Steal is FIFO across multiple threads.
 */
template<typename T>
class ConcurrentQueue {

public:

    ConcurrentQueue() = delete;
    ConcurrentQueue(Size capacity);
    
    ConcurrentQueue(const ConcurrentQueue &other) = delete;
    ConcurrentQueue(ConcurrentQueue &&other);
    
    ConcurrentQueue& operator= (const ConcurrentQueue &rhs) = delete;
    ConcurrentQueue& operator= (ConcurrentQueue &&rhs);

    ~ConcurrentQueue() = default;


    bool IsEmpty() const { return mHead == mTail - 1; }
    Size GetSize() const { return mTail - mHead; }
    Size GetCapacity() const { return mCapacity; }

    void Push(const T &value);
    bool Pop(T &out);
    bool Steal(T &out);


private:

    Array<T> mData;
    std::atomic<Size> mHead;
    std::atomic<Size> mTail;
    Size mCapacity;
};


template<typename T>
ConcurrentQueue<T>::ConcurrentQueue(Size capacity) : mData(capacity), mHead {0}, mTail {0}, mCapacity {capacity} {

    mData.Resize(capacity);
}


template<typename T>
ConcurrentQueue<T>::ConcurrentQueue(ConcurrentQueue &&other) {

    mData = std::move(other.mData);
    mHead = other.mHead.load(std::memory_order_relaxed);
    mTail = other.mTail.load(std::memory_order_relaxed);
    mCapacity = other.mCapacity;
}


template<typename T>
ConcurrentQueue<T>& ConcurrentQueue<T>::operator= (ConcurrentQueue &&other) {

    mData = std::move(other.mData);
    mHead = other.mHead.load(std::memory_order_relaxed);
    mTail = other.mTail.load(std::memory_order_relaxed);
    mCapacity = other.mCapacity;

    return *this;
}


template<typename T>
void ConcurrentQueue<T>::Push(const T &value) {

    Size tail = mTail.load(std::memory_order_relaxed);
    mData[ Math::ModuloPowerOfTwo(tail, mCapacity) ] = value;
    mTail.fetch_add(1, std::memory_order_release);
}


template<typename T>
bool ConcurrentQueue<T>::Pop(T &out) {

    Size tail = mTail.fetch_sub(1) - 1;
    Size head = mHead.load(std::memory_order_relaxed);

    // queue is empty -> reset tail to empty state
    if (head > tail)
    {
        mTail.store(head);
        return false;
    }

    // retrieve element
    out = mData[ Math::ModuloPowerOfTwo(tail, mCapacity) ];

    // not the last element in the queue -> ok to return
    if (head != tail)
    {
        return true;
    }

    // last element in queue -> check for preemption from steal operation
    // either way move mTail to new mHead position
    bool success = mHead.compare_exchange_weak(head, head + 1);
    ++mTail;

    return success;
}


template<typename T>
bool ConcurrentQueue<T>::Steal(T &out) {

    Size head = mHead.load(std::memory_order_acquire);
    Size tail = mTail.load(std::memory_order_relaxed);

    // queue is empty
    if (head >= tail)
    {
        return false;
    }
    
    out = mData[ Math::ModuloPowerOfTwo(head, mCapacity) ];

    // try to move head but check for preemption
    return mHead.compare_exchange_weak(head, head + 1);
}

    
} // Atuin
