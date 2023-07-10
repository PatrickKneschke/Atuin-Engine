
#pragma once


#include "Array.h"
#include "Core/Util/Types.h"
#include "Core/Debug/Log.h"

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <utility>


namespace Atuin {


template<typename T>
class Queue {

public:

    Queue();
    Queue(Size capacity);
    Queue(const std::initializer_list<T> &list);
    Queue(const Array<T> &array);
    Queue(Array<T> &&array);

    Queue(const Queue &other);
    Queue(Queue &&other);

    Queue& operator= (const Queue &rhs);
    Queue& operator= (Queue &&rhs);
    Queue& operator= (const std::initializer_list<T> &list);

    ~Queue() = default;
    

    bool IsEmpty() const {return mSize == 0;}
    Size GetSize() const { return mSize; }

    void Clear();
    void Push(const T &value);
    void Push(T &&value);
    void Pop();

    T&       Front();
    const T& Front() const;
    T&       Back();
    const T& Back() const;


private:

    void Expand();

    Array<T> mData;
    Size mHead;
    Size mTail;
    Size mSize;

    Log mLog;
};


template<typename T>
Queue<T>::Queue() : mData(), mHead{0}, mTail{0}, mSize{0}, mLog() {}


template<typename T>
Queue<T>::Queue(Size capacity) : mData(), mHead{0}, mTail{0}, mSize{0}, mLog() {

    mData.Resize(capacity);
}


template<typename T>
Queue<T>::Queue(const std::initializer_list<T> &list) : mData(list), mHead{0}, mTail{list.size()}, mSize{list.size()}, mLog() {}


template<typename T>
Queue<T>::Queue(const Array<T> &array) : mData(array), mHead{0}, mTail{array.GetSize()}, mSize{array.GetSize()}, mLog() {}


template<typename T>
Queue<T>::Queue(Array<T> &&array) : mData{std::move(array)}, mHead{0}, mTail{array.GetSize()}, mSize{array.GetSize()}, mLog() {}


template<typename T>
Queue<T>::Queue(const Queue &other) : mData{other.mData}, mHead{other.mHead}, mTail{other.mTail}, mSize{other.mSize}, mLog() {}


template<typename T>
Queue<T>::Queue(Queue &&other) : mData{std::move(other.mData)}, mHead{other.mHead}, mTail{other.mTail}, mSize{other.mSize}, mLog() {

    other.mHead = 0;
    other.mTail = 0;
    other.mSize = 0;
}


template<typename T>
Queue<T>& Queue<T>::operator= (const Queue &rhs) {

    mData = rhs.mData;
    mHead = rhs.mHead;
    mTail = rhs.mTail;
    mSize = rhs.mSize;
}
    

template<typename T>
Queue<T>& Queue<T>::operator= (Queue &&rhs) {

    mData = std::move(rhs.mData);
    mHead = rhs.mHead;
    mTail = rhs.mTail;
    mSize = rhs.mSize;

    rhs.mHead = 0;
    rhs.mTail = 0;
    rhs.mSize = 0;
}


template<typename T>
Queue<T>& Queue<T>::operator= (const std::initializer_list<T> &list) {

    mData = list;
    mHead = 0;
    mTail = list.size();
    mSize = list.size();
}


template<typename T>
void Queue<T>::Clear() {

    mHead = 0;
    mTail = 0;
    mSize = 0;
    mData.Clear();
}


template<typename T>
void Queue<T>::Push(const T &value) {

    if (mSize == mData.GetCapacity())
    {
        Expand();
    }

    mData[mTail] = value;
    mTail = (mTail + 1) % mData.GetCapacity();
    ++mSize;
}


template<typename T>
void Queue<T>::Push(T &&value) {

    if (mSize == mData.GetCapacity())
    {
        Expand();
    }

    mData[mTail] = std::move(value);
    mTail = (mTail + 1) % mData.GetCapacity();
    ++mSize;
}


template<typename T>
void Queue<T>::Pop() {

    if (mSize == 0)
    {
        return;
    }

    mHead = (mHead + 1) % mData.GetCapacity();
    --mSize;
}


template<typename T>
T& Queue<T>::Front() {

    if (mSize == 0)
    {
        mLog.Error( LogChannel::GENERAL, "Queue::Front called on empty queue." );
    }     

    return mData[mHead];
}


template<typename T>
const T& Queue<T>::Front() const {

    if (mSize == 0)
    {
        mLog.Error( LogChannel::GENERAL, "Queue::Front called on empty queue." );
    }  

    return mData[mHead];
}


template<typename T>
T& Queue<T>::Back() {

    if (mSize == 0)
    {
        mLog.Error( LogChannel::GENERAL, "Queue::Back called on empty queue." );
    }  

    if (mTail == 0)
    {
        return mData.Back();
    }

    return mData[mTail-1];
}


template<typename T>
const T& Queue<T>::Back() const {

    if (mSize == 0)
    {
        mLog.Error( LogChannel::GENERAL, "Queue::Back called on empty queue." );
    }  

    if (mTail == 0)
    {
        return mData.Back();
    }

    return mData[(mTail-1)];
}


template<typename T>
void Queue<T>::Expand() {

    Array<T> temp;
    temp.Resize( Math::NextPowerOfTwo(mData.GetCapacity()) );
    for (Size i = 0; i<mSize; i++)
    {
        temp[i] = std::move(mData[ (mHead + i) % mSize ]);
    }
    mData = std::move(temp);
    mHead = 0;
    mTail = mSize;
}

   
} // Atuin
