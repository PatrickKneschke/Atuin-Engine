/*
 
 Type definitions and global constants used throughout the engine.

 */

#pragma once


#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>


namespace Atuin {


using Byte  = uint8_t;
using Size  = size_t;

constexpr Size KB   = 1024;
constexpr Size MB   = KB * KB;
constexpr Size GB   = KB * MB;

inline Size operator""_KB (unsigned long long x) { return x * KB; }
inline Size operator""_MB (unsigned long long x) { return x * MB; }
inline Size operator""_GB (unsigned long long x) { return x * GB; }

using U8    = uint8_t;
using U16   = uint16_t;
using U32   = uint32_t;
using U64   = uint64_t;

constexpr U8  U8_MAX    = UINT8_MAX;
constexpr U16 U16_MAX   = UINT16_MAX;
constexpr U32 U32_MAX   = UINT32_MAX;
constexpr U64 U64_MAX   = UINT64_MAX;

using I8    = int8_t;
using I16   = int16_t;
using I32   = int32_t;
using I64   = int64_t;

constexpr I8  I8_MIN    = INT8_MIN;
constexpr I16 I16_MIN   = INT16_MIN;
constexpr I32 I32_MIN   = INT32_MIN;
constexpr I64 I64_MIN   = INT64_MIN;

constexpr I8  I8_MAX    = INT8_MAX;
constexpr I16 I16_MAX   = INT16_MAX;
constexpr I32 I32_MAX   = INT32_MAX;
constexpr I64 I64_MAX   = INT64_MAX;

using UPtr = uintptr_t;


struct PairHash {
    
    template<class T1, class T2>
    Size operator () (const std::pair<T1,T2> &p) const {

        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        return h1 ^ (h2 - 1);
    }
};


} // Atuin