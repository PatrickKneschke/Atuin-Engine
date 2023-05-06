
#pragma once


#include "Types.h"


namespace Atuin {
namespace Math {


template<typename T>
struct Less {

    bool operator() (const T &a, const T &b) {

        return a < b;
    }
};


template<typename T>
struct Greater {

    bool operator() (const T &a, const T &b) {

        return a > b;
    }
};


bool IsPowerOfTwo(U64 n);
U64 NextPowerOfTwo(U64 n);
U64 PrevPowerOfTwo(U64 n);
U64 ClosestPowerOfTwo(U64 n);
U64 ModuloPowerOfTwo(U64 n, U64 mod);


} // Math  
} // Atuin
