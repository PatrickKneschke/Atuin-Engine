
#include "Math.h"

#include <assert.h>
#include <bit>


namespace Atuin {
namespace Math {


bool IsPowerOfTwo(U64 n) {

    return n > 0 && (n & (n - 1)) == 0;
}


U64 NextPowerOfTwo(U64 n) {

    if (n >= ((U64)1<<63))
    {
        return n;
    }

    return std::bit_ceil(n+1);
}


U64 PrevPowerOfTwo(U64 n) {

    if (n <= 1)
    {
        return n;
    }

    return std::bit_floor(n-1);
}


U64 ClosestPowerOfTwo(U64 n) {

    if (IsPowerOfTwo(n))
    {
        return n;
    }

    U64 next = NextPowerOfTwo(n);
    U64 prev = PrevPowerOfTwo(n);

    if (next-n < n-prev)
    {
        return next;
    }

    return prev;
}


U64 ModuloPowerOfTwo(U64 n, U64 mod) {

    assert( IsPowerOfTwo(mod) );

    return n & (mod - 1);
}


} // Math    
} // Atuin
