
#pragma once


#include "Types.h"

#include <assert.h>
#include <bit>


namespace Atuin {
namespace Math {


bool IsPowerOfTwo(U64 n) {

    return (n & (n - 1)) == 0;
}


U64 NextPowerOfTwo(U64 n) {

    return std::bit_ceil(n);
}


U64 PrevPowerOfTwo(U64 n) {

    return std::bit_floor(n);
}


U64 ClosestPowerOfTwo(U64 n) {

    U64 top = std::bit_ceil(n);
    U64 bottom = std::bit_floor(n);

    if (top-n < n-bottom)
    {
        return top;
    }

    return bottom;
}


U64 ModuloPowerOfTwo(U64 n, U64 mod) {

    assert( IsPowerOfTwo(mod) );

    return n & (mod - 1);
}


} // Math  
} // Atuin
