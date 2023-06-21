
#pragma once


#include "Core/Util/Types.h"
#include "Core/Util/StringFormat.h"
#include "Core/DataStructures/Array.h"


namespace Atuin {


class MemoryManager;


/*
    KeyType is any type for which the standard hash struct template is implemented
*/
template<typename KeyType, typename ValueType>
class Map {

    struct MapData {

        KeyType key;
        ValueType value;
        MapData *prev;
        MapData *next;

        MapData(KeyType k = KeyType(), ValueType v = ValueType(), MapData *p = nullptr, MapData *n = nullptr) : key{k}, value{v}, prev{p}, next{n} {}
    };

public:
    Map(MemoryManager *memory = nullptr);
    Map(Size numBuckets, float maxLoadFactor, MemoryManager *memory = nullptr);
    Map(const Map &other, MemoryManager *memory = nullptr);
    Map(Map &&other);

    Map& operator= (const Map &rhs);
    Map& operator= (Map &&rhs);

    Size GetSize() const { return mSize; }
    Size GetNumBuckets() const { return mNumBuckets; }
    float GetMaxLoadFactor() const { return mMaxLoadFactor; }

    MapData* Find(KeyType key);
    void Insert(KeyType key, ValueType value);
    void Erase(KeyType key);
    void Clear();

    ValueType& At(KeyType key);
    const ValueType& At(KeyType key) const;
    ValueType& operator[] (KeyType key);


private:

    Size Hash(KeyType key);
    void Rehash();

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
    mMaxLoadFactor {1.0f} 
{

}


template<typename KeyType, typename ValueType>
Map<KeyType, ValueType>::Map(Size numBuckets, float maxLoadFactor, MemoryManager *memory) : 
    mBuckets(numBuckets, nullptr, memory), 
    mNumBuckets {numBuckets}, 
    mSize {0},
    mMaxLoadFactor {maxLoadFactor} 
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
    for (Size i = 0; i < mNumBuckets; i++)
    {
        MapData *head = other.mBuckets[i];
        while (head != nullptr)
        {
            MapData *newHead;
            if (pMemory != nullptr)
            {
                newHead = pMemory->New<MapData>(head->key, head->value, nullptr, mBuckets[i]);
            }
            else
            {
                newHead = new MapData(head->key, head->value, nullptr, mBuckets[i]);
            }
            mBuckets[i]->prev = newHead;
            mBuckets[i] = newHead;
            head = head->next;
            ++mSize;
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
    for (Size i = 0; i < mNumBuckets; i++)
    {
        MapData *head = rhs.mBuckets[i];
        while (head != nullptr)
        {
            MapData *newHead;
            if (pMemory != nullptr)
            {
                newHead = pMemory->New<MapData>(head->key, head->value, nullptr, mBuckets[i]);
            }
            else
            {
                newHead = new MapData(head->key, head->value, nullptr, mBuckets[i]);
            }
            mBuckets[i]->prev = newHead;
            mBuckets[i] = newHead;
            head = head->next;
            ++mSize;
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
Map<KeyType, ValueType>::MapData* Map<KeyType, ValueType>::Find(KeyType key) {

    Size hash = Hash(key);
    MapData *head = mBuckets[hash];
    while(head != nullptr) 
    {
        if (head->key == key)
        {
            return head;
        }
        head = head->next;
    }

    return nullptr;
}


template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::Insert(KeyType key, ValueType value) {
    
    Size hash = Hash(key);
    MapData *head = mBuckets[hash];
    while (head != nullptr)
    {
        if (head->key == key)
        {
            head->value = value;
            return;
        }
        head = head->next;
    }
    
    MapData *oldHead = mBuckets[hash];
    if (pMemory != nullptr)
    {
        mBuckets[hash] = pMemory->New<MapData>(key, value, nullptr, oldHead);
    }
    else 
    {
        mBuckets[hash] = new MapData(key, value, nullptr, oldHead);
    }
    
    if (oldHead != nullptr) 
    {
        oldHead->prev = mBuckets[hash];
    }

    ++mSize;

    Rehash();
}


template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::Erase(KeyType key) {

    Size hash = Hash(key);

    MapData *head = mBuckets[hash];
    while (head != nullptr)
    {
        if (head->key == key)
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

            if (pMemory != nullptr)
            {
                pMemory->Delete(head);
            }
            else
            {
                delete head;
            }

            --mSize;

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

            if (pMemory != nullptr)
            {
                pMemory->Delete(head);
            }
            else
            {
                delete head;
            }

            if(mBuckets[i] != nullptr)
            {
                mBuckets[i]->prev = nullptr;
            }

            head = mBuckets[i];
        }        
    }
    mSize = 0;
}


template<typename KeyType, typename ValueType>
ValueType& Map<KeyType, ValueType>::At(KeyType key) {

    Size hash = Hash(key);
    MapData *head = mBuckets[hash];
    while (head != nullptr)
    {
        if (head->key == key)
        {
            return head->value;
        }
        head = head->next;
    }

    throw std::out_of_range( FormatStr("Trying to access element with key \"%s\", which is not in the map", key) );    
}


template<typename KeyType, typename ValueType>
const ValueType& Map<KeyType, ValueType>::At(KeyType key) const {

    Size hash = Hash(key);
    MapData *head = mBuckets[hash];
    while (head != nullptr)
    {
        if (head->key == key)
        {
            return head->value;
        }
        head = head->next;
    }

    throw std::out_of_range( FormatStr("Trying to access element with key \"%s\", which is not in the map", key) );    
}


template<typename KeyType, typename ValueType>
ValueType& Map<KeyType, ValueType>::operator[](KeyType key) {

    Size hash = Hash(key);
    MapData *head = mBuckets[hash];
    while (head != nullptr)
    {
        if (head->key == key)
        {
            return head->value;
        }
        head = head->next;
    }

    if (pMemory != nullptr)
    {
        mBuckets[hash] = pMemory->New<MapData>(key, ValueType(), nullptr, mBuckets[hash]);
    }
    else
    {
        mBuckets[hash] = new MapData(key, ValueType(), nullptr, mBuckets[hash]);
    }
    ++mSize;

    Rehash();

    return mBuckets[ Hash(key) ]->value;
}


template<typename KeyType, typename ValueType>
Size Map<KeyType, ValueType>::Hash(KeyType key) {

    return std::hash<KeyType>{}(key) % mNumBuckets;
}


template<typename KeyType, typename ValueType>
void Map<KeyType, ValueType>::Rehash() {

    if (mSize <= (Size)(mMaxLoadFactor * (float)mNumBuckets)) {

        return;
    }

    mNumBuckets *= 2;
    Array<MapData*> newBuckets(mNumBuckets, nullptr, pMemory);
    for (Size i = 0; i < mNumBuckets; i++)
    {
        MapData *head = mBuckets[i];
        while (head != nullptr)
        {
            Size hash = Hash(head->key);

            MapData *temp = head->next;

            head->prev = nullptr;
            head->next = newBuckets[hash];
            newBuckets[hash]->prev = head;
            newBuckets[hash] = head;

            head = temp;
        }        
    }

    mBuckets = std::move(newBuckets);
}


    
} // Atuin
