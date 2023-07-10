
#pragma once


#include "Core/Util/Types.h"
#include "Core/Util/StringFormat.h"
#include "Core/Memory/Memory.h"
#include "Core/Debug/Log.h"
#include "Core/DataStructures/Array.h"

#include <iostream>


namespace Atuin {

/*
    KeyType is any type for which the standard hash struct template is implemented
*/
template<typename KeyType, typename ValueType>
class Map {
    
    struct MapData {

        std::pair<const KeyType, ValueType> data;
        MapData *prev;
        MapData *next;

        MapData(const KeyType &k = KeyType(), ValueType v = ValueType(), MapData *p = nullptr, MapData *n = nullptr) : data{k, v}, prev{p}, next{n} {}
    };

public:

    class iterator {

        public:

            explicit iterator(Array<MapData*> *data, Size bucket = 0) : pData {data}, mBucket {bucket}, mNode {nullptr} {

                while (mNode == nullptr && mBucket < pData->GetSize())
                {
                    mNode = (*pData)[mBucket++];
                }
            }
            
            iterator operator++() {
            
                mNode = mNode->next;
                while (mNode == nullptr && ++mBucket < pData->GetSize())
                {
                    mNode = (*pData)[mBucket];
                }

                return *this;
            }

            iterator operator++(int) {

                iterator it = *this;

                mNode = mNode->next;
                while (mNode == nullptr && ++mBucket < pData->GetSize())
                {
                    mNode = (*pData)[mBucket];
                }

                return it;
            }

            std::pair<const KeyType, ValueType>& operator*() const { return mNode->data; }
            std::pair<const KeyType, ValueType>* operator->() const { return &(mNode->data); }

            bool operator==(const iterator &rhs) const { return mNode == rhs.mNode; }
            bool operator!=(const iterator &rhs) const { return mNode != rhs.mNode; }

        private:
            Array<MapData*> *pData;
            Size mBucket;
            MapData *mNode;
    };

    class const_iterator {

        public:

            explicit const_iterator(const Array<MapData*> *data, Size bucket = 0) : pData {data}, mBucket {bucket}, mNode {nullptr} {

                while (mNode == nullptr && mBucket < pData->GetSize())
                {
                    mNode = (*pData)[mBucket++];
                }
            }
            
            const_iterator operator++() {
            
                mNode = mNode->next;
                while (mNode == nullptr && ++mBucket < pData->GetSize())
                {
                    mNode = (*pData)[mBucket];
                }

                return *this;
            }

            const_iterator operator++(int) {

                const_iterator it = *this;

                mNode = mNode->next;
                while (mNode == nullptr && ++mBucket < pData->GetSize())
                {
                    mNode = (*pData)[mBucket];
                }

                return it;
            }

            const std::pair<const KeyType, ValueType>& operator*() const { return mNode->data; }
            const std::pair<const KeyType, ValueType>* operator->() const { return &(mNode->data); }

            bool operator==(const const_iterator &rhs) const { return mNode == rhs.mNode; }
            bool operator!=(const const_iterator &rhs) const { return mNode != rhs.mNode; }

        private:
            const Array<MapData*> *pData;
            Size mBucket;
            MapData *mNode;
    };


public:
    Map();
    Map(Size numBuckets, float maxLoadFactor);
    Map(const Map &other);
    Map(Map &&other);

    Map& operator= (const Map &rhs);
    Map& operator= (Map &&rhs);

    ~Map();

    bool IsEmpty() const { return mSize == 0; }
    Size GetSize() const { return mSize; }
    Size GetNumBuckets() const { return mNumBuckets; }
    float GetMaxLoadFactor() const { return mMaxLoadFactor; }

    iterator Find(const KeyType &key);
    const_iterator Find(const KeyType &key) const;
    void Insert(const KeyType &key, const ValueType &value);
    void Erase(const KeyType &key);
    void Clear();

    ValueType& At(const KeyType &key);
    const ValueType& At(const KeyType &key) const;
    ValueType& operator[] (const KeyType &key);

    iterator Begin() { return iterator(&mBuckets); }
    const_iterator Cbegin() const { return const_iterator(&mBuckets); }
    iterator End() { return iterator(&mBuckets, mBuckets.GetSize()); }
    const_iterator Cend() const { return const_iterator(&mBuckets, mBuckets.GetSize()); }


private: 

    Size Hash(const KeyType &key) const;
    void Rehash();
    MapData* MakeNode(const KeyType &key, const ValueType &value, MapData *prev = nullptr, MapData *next = nullptr);
    void DeleteNode(MapData *node);

    Array<MapData*> mBuckets;
    Size mNumBuckets;
    Size mSize;
    float mMaxLoadFactor;

    Log mLog;
    Memory mMemory;
};


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::Map() : 
    mBuckets(64, nullptr), 
    mNumBuckets {64}, 
    mSize {0}, 
    mMaxLoadFactor {1.0f},
    mLog(), 
    mMemory() 
{
    
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::Map(Size numBuckets, float maxLoadFactor) : 
    mBuckets(numBuckets, nullptr), 
    mNumBuckets {numBuckets}, 
    mSize {0},
    mMaxLoadFactor {maxLoadFactor},
    mLog(), 
    mMemory() 
{
    
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::Map(const Map &other) :
    mBuckets(other.mNumBuckets, nullptr), 
    mNumBuckets {other.mNumBuckets}, 
    mSize {0}, 
    mMaxLoadFactor {other.mMaxLoadFactor},
    mLog(), 
    mMemory() 
{
    // make deep copy of other map
    for (Size i = 0; i < other.mNumBuckets; i++)
    {
        MapData *head = other.mBuckets[i];
        while (head != nullptr)
        {
            MapData *newHead = MakeNode(head->data.first, head->data.second, nullptr, mBuckets[i]);
            if (mBuckets[i] != nullptr)
            {
                mBuckets[i]->prev = newHead;
            }            
            mBuckets[i] = newHead;
            head = head->next;
        }
    }
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::Map(Map &&other) :
    mBuckets(std::move(other.mBuckets)), 
    mNumBuckets {other.mNumBuckets}, 
    mSize {other.mSize}, 
    mMaxLoadFactor {other.mMaxLoadFactor},
    mLog(), 
    mMemory(other.mMemory) 
{
    other.mNumBuckets = 0;
    other.mSize = 0;
    other.mMaxLoadFactor = 0;
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>& Map<KeyType, ValueType>::operator=(const Map &rhs) {

    if (this != &rhs)
    {
        Clear();

        mNumBuckets = rhs.mNumBuckets;
        mMaxLoadFactor = rhs.mMaxLoadFactor;    
        mBuckets = Array<MapData*>(mNumBuckets, nullptr); 
        for (Size i = 0; i < rhs.mNumBuckets; i++)
        {
            MapData *head = rhs.mBuckets[i];
            while (head != nullptr)
            {
                MapData *newHead = MakeNode(head->data.first, head->data.second, nullptr, mBuckets[i]);
                if (mBuckets[i] != nullptr)
                {
                    mBuckets[i]->prev = newHead;
                }
                mBuckets[i] = newHead;
                head = head->next;
            }
        }
    }
    
    return *this;
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>& Map<KeyType, ValueType>::operator=(Map &&rhs) {

    if (this != &rhs) {

        Clear();

        mBuckets = std::move(rhs.mBuckets); 
        mNumBuckets = rhs.mNumBuckets; 
        mSize = rhs.mSize;
        mMaxLoadFactor = rhs.mMaxLoadFactor; 
        mMemory = rhs.mMemory;

        rhs.mNumBuckets = 0;
        rhs.mSize = 0;
        rhs.mMaxLoadFactor = 0;
    }

    return *this;
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::~Map() {
    
    Clear();
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::iterator Map<KeyType, ValueType>::Find(const KeyType &key) {

    Size hash = Hash(key);
    iterator it = iterator(&mBuckets, hash);
    iterator end = End();
    while (it != end && it->first != key)
    {
        ++it;
    }

    return it;
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::const_iterator Map<KeyType, ValueType>::Find(const KeyType &key) const {

    Size hash = Hash(key);
    const_iterator it = const_iterator(&mBuckets, hash);
    const_iterator end = Cend();
    while (it != end && it->first != key)
    {
        ++it;
    }

    return it;
}


template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::Insert(const KeyType &key, const ValueType &value) {

    iterator it = Find(key);
    if (it != End())
    {
        it->second = value;
        return;
    }
    
    Rehash();

    Size hash = Hash(key);
    MapData *oldHead = mBuckets[hash];
    mBuckets[hash] = MakeNode(key, value, nullptr, oldHead);    
    if (oldHead != nullptr) 
    {
        oldHead->prev = mBuckets[hash];
    }
}


template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::Erase(const KeyType &key) {

    Size hash = Hash(key);
    MapData *head = mBuckets[hash];
    while (head != nullptr)
    {
        if (head->data.first == key)
        {
            MapData *prev = head->prev, *next = head->next;
            if (next != nullptr)
            {
                next->prev = prev;
            }
            if (prev != nullptr)
            {
                prev->next = next;
            }
            else
            {
                mBuckets[hash] = next;
            }
            DeleteNode(head);

            return;
        }
        head = head->next;    
    }
}


template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::Clear() {

    for (Size i = 0; i<mNumBuckets; i++) {

        MapData *head = mBuckets[i];
        while (head != nullptr)
        {
            mBuckets[i] = head->next;
            DeleteNode(head);

            if(mBuckets[i] != nullptr)
            {
                mBuckets[i]->prev = nullptr;
            }

            head = mBuckets[i];
        }        
    }
}


template<typename KeyType, typename ValueType>
ValueType& Map<KeyType, ValueType>::At(const KeyType &key) {

    iterator it = Find(key);
    if (it == End())
    {
        mLog.Error( LogChannel::GENERAL, FormatStr("Trying to access element with key \"%s\", which is not in the map", key) );  
    }

    return it->second;
}


template<typename KeyType, typename ValueType>
const ValueType& Map<KeyType, ValueType>::At(const KeyType &key) const {

    const_iterator it = Find(key);
    if (it == Cend())
    {
        mLog.Error( LogChannel::GENERAL, FormatStr("Trying to access element with key \"%s\", which is not in the map", key) );
    }

    return it->second;
}


template<typename KeyType, typename ValueType>
ValueType& Map<KeyType, ValueType>::operator[](const KeyType &key) {

    iterator it = Find(key);
    if (it != End())
    {
        return it->second;
    }

    Rehash();

    Size hash = Hash(key);
    mBuckets[hash] = MakeNode(key, ValueType(), nullptr, mBuckets[hash]);

    return mBuckets[hash]->data.second;
}


template<typename KeyType, typename ValueType>
Size Map<KeyType, ValueType>::Hash(const KeyType &key) const {

    assert(mNumBuckets > 0);

    return std::hash<KeyType>{}(key) % mNumBuckets;
}


template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::Rehash() {

    if (mSize < (Size)(mMaxLoadFactor * (float)mNumBuckets)) {

        return;
    }

    Array<MapData*> newBuckets(2*mNumBuckets, nullptr);
    for (Size i = 0; i < mNumBuckets; i++)
    {
        MapData *head = mBuckets[i];
        while (head != nullptr)
        {
            MapData *temp = head->next;
            
            Size hash = Hash(head->data.first);
            head->prev = nullptr;
            head->next = newBuckets[hash];
            if (newBuckets[hash] != nullptr)
            {
                newBuckets[hash]->prev = head;
            }            
            newBuckets[hash] = head;

            head = temp;
        }        
    }

    mNumBuckets *= 2;
    mBuckets = std::move(newBuckets);
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::MapData* Map<KeyType, ValueType>::MakeNode(const KeyType &key, const ValueType &value, MapData *prev, MapData *next) {

    MapData *node = mMemory.New<MapData>(key, value, prev, next);
    ++mSize;

    return node;
}


template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::DeleteNode(MapData *node) {

    mMemory.Delete(node);
    node = nullptr;
    --mSize;
}



template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::iterator begin(Map<KeyType, ValueType> &mp) { return mp.Begin(); }

template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::const_iterator begin(const Map<KeyType, ValueType> &mp) { return mp.Cbegin(); }

template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::iterator end(Map<KeyType, ValueType> &mp) { return mp.End(); }

template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::const_iterator end(const Map<KeyType, ValueType> &mp) { return mp.Cend(); }

    
} // Atuin
