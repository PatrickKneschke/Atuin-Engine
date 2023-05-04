
#pragma once


#include "Array.h"
#include "Core/Util/Types.h"


namespace Atuin {


template<typename T>
class Queue {

public:

    Queue();


private:

    Array<T> mData;
    Size mHead;
    Size mTail;
};

    
} // Atuin
