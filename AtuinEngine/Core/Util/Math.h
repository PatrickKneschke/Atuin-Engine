
#pragma once


#include "Types.h"


namespace Atuin {
namespace Math {


bool IsPowerOfTwo(U64 n);
U64 NextPowerOfTwo(U64 n);
U64 PrevPowerOfTwo(U64 n);
U64 ClosestPowerOfTwo(U64 n);
U64 ModuloPowerOfTwo(U64 n, U64 mod);


} // Math  
} // Atuin
