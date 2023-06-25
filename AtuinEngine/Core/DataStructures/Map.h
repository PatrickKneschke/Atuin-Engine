
#pragma once


#include "Core/Util/Types.h"
#include "Core/Util/StringFormat.h"
#include "Core/DataStructures/Array.h"

#include <iostream>


namespace Atuin {


class MemoryManager;

/*
    KeyType is any type for which the standard hash struct template is implemented
*/
template<typename KeyType, typename ValueType>
class Map {

    // TODO make KeyType const and define const versions of all functions where needed
    struct MapData {

        std::pair<KeyType, ValueType> data;
        MapData *prev;
        MapData *next;

        MapData(const KeyType &k = KeyType(), ValueType v = ValueType(), MapData *p = nullptr, MapData *n = nullptr) : data{k, v}, prev{p}, next{n} {}
    };

public:

    class iterator {

        public:

            explicit iterator(MapData **first, Size numBuckets, MapData **head, MapData *node) : mFirst {first}, mNumBuckets {numBuckets}, mHead {head}, mNode {node} {}
            
            iterator operator++() {
            
                mNode = mNode->next;
                while (mNode == nullptr && mHead < mFirst + mNumBuckets)
                {
                    mNode = *(++mHead);
                }

                return *this;
            }

            iterator operator++(int) {
            
                iterator it = *this;

                mNode = mNode->next;
                while (mNode == nullptr && mHead < mFirst + mNumBuckets)
                {
                    mNode = *(++mHead);
                }

                return it;
            }

            iterator operator--() {

                mNode = mNode->next;
                while (mNode == nullptr && mHead >= mFirst)
                {
                    mNode = *(--mHead);
                }

                return *this;
            }

            iterator operator--(int) {

                iterator it = *this;

                mNode = mNode->next;
                while (mNode == nullptr && mHead >= mFirst)
                {
                    mNode = *(--mHead);
                }

                return it;
            }

            std::pair<KeyType, ValueType>& operator*() { return mNode->data; }
            std::pair<KeyType, ValueType>* operator->() { return &(mNode->data); }

            bool operator==(const iterator &rhs) { return mNode == rhs.mNode; }
            bool operator!=(const iterator &rhs) { return mNode != rhs.mNode; }

        private:
            MapData **mFirst;
            Size mNumBuckets;
            MapData **mHead;
            MapData *mNode;
    };

    class const_iterator {

        public:

            explicit const_iterator(MapData **first, Size numBuckets, MapData **head, MapData *node) : mFirst {first}, mNumBuckets {numBuckets}, mHead {head}, mNode {node} {}

            const_iterator operator++() {
            
                mNode = mNode->next;
                while (mNode == nullptr && mHead < mFirst + mNumBuckets)
                {
                    mNode = *(++mHead);
                }

                return *this;
            }

            const_iterator operator++(int) {
            
                const_iterator it = *this;

                mNode = mNode->next;
                while (mNode == nullptr && mHead < mFirst + mNumBuckets)
                {
                    mNode = *(++mHead);
                }

                return it;
            }

            const_iterator operator--() {

                mNode = mNode->next;
                while (mNode == nullptr && mHead >= mFirst)
                {
                    mNode = *(--mHead);
                }

                return *this;
            }

            const_iterator operator--(int) {

                const_iterator it = *this;

                mNode = mNode->next;
                while (mNode == nullptr && mHead >= mFirst)
                {
                    mNode = *(--mHead);
                }

                return it;
            }

            const std::pair<KeyType, ValueType>& operator*() { return mNode->data; }
            const std::pair<KeyType, ValueType>* operator->() { return &(mNode->data); }

            bool operator==(const const_iterator &rhs) { return mNode == rhs.mNode; }
            bool operator!=(const const_iterator &rhs) { return mNode != rhs.mNode; }

        private:
            MapData **mFirst;
            Size mNumBuckets;
            MapData **mHead;
            MapData *mNode;
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    Map(MemoryManager *memory = nullptr);
    Map(Size numBuckets, float maxLoadFactor, MemoryManager *memory = nullptr);
    Map(const Map &other, MemoryManager *memory = nullptr);
    Map(Map &&other);

    Map& operator= (const Map &rhs);
    Map& operator= (Map &&rhs);

    ~Map();

    Size GetSize() const { return mSize; }
    Size GetNumBuckets() const { return mNumBuckets; }
    float GetMaxLoadFactor() const { return mMaxLoadFactor; }

    iterator Find(const KeyType &key);
    void Insert(const KeyType &key, const ValueType &value);
    void Erase(const KeyType &key);
    void Clear();

    ValueType& At(const KeyType &key);
    const ValueType& At(const KeyType &key) const;
    ValueType& operator[] (const KeyType &key);

    iterator Begin()  { 
        
        Size i = 0;
        for ( ; i<mNumBuckets && mBuckets[i]==nullptr; i++);

        return iterator(mBuckets.Data(), mNumBuckets, mBuckets.Data() + i,  i < mNumBuckets ? mBuckets[i] : nullptr); 
    }
    const_iterator Cbegin()  { 
        
        Size i = 0;
        for ( ; i<mNumBuckets && mBuckets[i]==nullptr; i++);

        return const_iterator(mBuckets.Data(), mNumBuckets, mBuckets.Data() + i,  i < mNumBuckets ? mBuckets[i] : nullptr); 
    }
    iterator         End()    { return iterator(mBuckets.Data(), mNumBuckets, mBuckets.Data() + mNumBuckets, nullptr); }
    const_iterator   Cend()   { return const_iterator(mBuckets.Data(), mNumBuckets, mBuckets.Data() + mNumBuckets, nullptr); }


private: 

    Size Hash(const KeyType &key);
    void Rehash();
    MapData* MakeNode(const KeyType &key, const ValueType &value, MapData *prev = nullptr, MapData *next = nullptr);
    void DeleteNode(MapData *node);

    Array<MapData*> mBuckets;
    Size mNumBuckets;
    Size mSize;
    float mMaxLoadFactor;

    MemoryManager *pMemory;
};


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::Map(MemoryManager *memory) : 
    mBuckets(64, nullptr, memory), 
    mNumBuckets {64}, 
    mSize {0}, 
    mMaxLoadFactor {1.0f},
    pMemory {memory}
{
    
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::Map(Size numBuckets, float maxLoadFactor, MemoryManager *memory) : 
    mBuckets(numBuckets, nullptr, memory), 
    mNumBuckets {numBuckets}, 
    mSize {0},
    mMaxLoadFactor {maxLoadFactor} ,
    pMemory {memory}
{
    
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::Map(const Map &other, MemoryManager *memory) :
    mBuckets(other.mNumBuckets, nullptr, memory), 
    mNumBuckets {other.mNumBuckets}, 
    mSize {0}, 
    mMaxLoadFactor {other.mMaxLoadFactor}, 
    pMemory {memory}
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
    pMemory {other.pMemory}
{
    other.mNumBuckets = 0;
    other.mSize = 0;
    other.mMaxLoadFactor = 0;
    other.pMemory = nullptr;
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>& Map<KeyType, ValueType>::operator=(const Map &rhs) {

    Clear();

    mNumBuckets = rhs.mNumBuckets;
    mMaxLoadFactor = rhs.mMaxLoadFactor;    
    mBuckets = Array<MapData*>(mNumBuckets, nullptr, pMemory); 
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


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>& Map<KeyType, ValueType>::operator=(Map &&rhs) {

    mBuckets = std::move(rhs.mBuckets); 
    mNumBuckets = rhs.mNumBuckets; 
    mSize = rhs.mSize;
    mMaxLoadFactor = rhs.mMaxLoadFactor; 
    pMemory = rhs.pMemory;

    rhs.mNumBuckets = 0;
    rhs.mSize = 0;
    rhs.mMaxLoadFactor = 0;
    rhs.pMemory = nullptr;
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::~Map() {
    
    Clear();
}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::iterator Map<KeyType, ValueType>::Find(const KeyType &key) {

    Size hash = Hash(key);
    MapData *head = mBuckets[hash];
    while(head != nullptr) 
    {
        if (head->data.first == key)
        {
            return iterator(mBuckets.Data(), mNumBuckets, mBuckets.Data() + hash, head);
        }
        head = head->next;
    }

    return End();
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
        throw std::out_of_range( FormatStr("Trying to access element with key \"%s\", which is not in the map", key) );    
    }

    return it->second;
}


template<typename KeyType, typename ValueType>
const ValueType& Map<KeyType, ValueType>::At(const KeyType &key) const {

    iterator it = Find(key);
    if (it == End())
    {
        throw std::out_of_range( FormatStr("Trying to access element with key \"%s\", which is not in the map", key) );    
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
Size Map<KeyType, ValueType>::Hash(const KeyType &key) {

    assert(mNumBuckets > 0);

    return std::hash<KeyType>{}(key) % mNumBuckets;
}


template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::Rehash() {

    if (mSize < (Size)(mMaxLoadFactor * (float)mNumBuckets)) {

        return;
    }

    Array<MapData*> newBuckets(2*mNumBuckets, nullptr, pMemory);
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

    MapData *node = nullptr;
    if (pMemory != nullptr)
    {
        node = pMemory->New<MapData>(key, value, prev, next);
    }
    else
    {
        node = new MapData(key, value, prev, next);
    }
    ++mSize;

    return node;
}


template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::DeleteNode(MapData *node) {

    if (pMemory != nullptr)
    {
        pMemory->Delete(node);
    }
    else
    {
        delete node;
    }
    node = nullptr;
    --mSize;
}



template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::iterator begin(Map<KeyType, ValueType> &mp) { return mp.Begin(); }

template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::iterator end(Map<KeyType, ValueType> &mp) { return mp.End(); }

    
} // Atuin
