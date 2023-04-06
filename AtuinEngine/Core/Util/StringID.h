
/*
 
    String hash using the FNV-1a hash function.
 
 */

#pragma once


#include <cstddef>
#include <cstdint>


namespace Atuin {


constexpr uint64_t StringIdHashConcat(uint64_t base,  const char *str) {

    if( !(*str) )
    {
        return base;
    }

    return StringIdHashConcat( (base ^ *str) * 0x100000001b3, str + 1 );
}

constexpr uint64_t StringIdHash(const char *str) {

    return StringIdHashConcat(0xcbf29ce484222325, str);
}


inline uint64_t operator""_SID (const char *str, size_t) { return StringIdHash(str); }


#define SID(str) StringIdHash(str)


} // Atuin