
#include <catch2/catch.hpp>

#include "Core/DataStructures/Map.h"

#include <iostream>


using namespace Atuin;


TEST_CASE("create a map", "[map]") {

	Map<int, int> mp;
    Map<float, int> mp2(54, 1.2f);

    REQUIRE(mp.GetSize() == 0);
    REQUIRE(mp2.GetSize() == 0);
}


TEST_CASE("copy a map", "[map]") {

	Map<std::string, int> mp(16, 0.8f);
    mp["first"] = 1;
    UPtr ptr1 = reinterpret_cast<UPtr>(mp.Find("first"));

    Map<std::string, int> mp2(mp);
    UPtr ptr2 = reinterpret_cast<UPtr>(mp2.Find("first"));

    REQUIRE(mp2.GetSize() == 1);
    REQUIRE(mp2["first"] == 1);
    REQUIRE(ptr2 != ptr1);

    Map<std::string, int> mp3(std::move(mp));
    UPtr ptr3 = reinterpret_cast<UPtr>(mp3.Find("first"));

    REQUIRE(mp3.GetSize() == 1);
    REQUIRE(mp3.GetNumBuckets() == 16);
    REQUIRE(mp3.GetMaxLoadFactor() == 0.8f);
    REQUIRE(mp3.At("first") == 1);
    REQUIRE(ptr3 == ptr1);

    REQUIRE(mp.GetSize() == 0);
    REQUIRE(mp.GetNumBuckets() == 0);
    REQUIRE(mp.GetMaxLoadFactor() == 0);
}


    struct Foo {

        float f;
        int i;

        bool operator== (const Foo &rhs) {

            return f == rhs.f && i == rhs.i;
        }
    };

    template<>
    struct std::hash<Foo> {

        std::size_t operator()(Foo const& foo) const noexcept {

            std::size_t h1 = std::hash<float>{}(foo.f);
            std::size_t h2 = std::hash<int>{}(foo.i);
            return h1 ^ (h2 << 1);
        }
    };

TEST_CASE("custom hash", "[map]") {

    Map<Foo, float> mp;
    mp[{4.6f, 13}] = 37.89f;
    REQUIRE_NOTHROW(mp.At({4.6f, 13}));
}


TEST_CASE("modify map", "[map]") {

    Map<U32, float> mp;
    mp[1] = 2.f;
    mp[2] = 4.f;
    mp[3] = 8.f;

    mp.Insert(2, 5.f);
    REQUIRE(mp.GetSize() == 3);
    REQUIRE(mp[2] == 5.f);

    mp.Insert(4, 16);
    REQUIRE(mp.GetSize() == 4);
    
    mp.Erase(2);
    REQUIRE(mp.GetSize() == 3);

    mp[1] = 1.f;
    REQUIRE(mp[1] == 1.f);

    mp.Clear();
    REQUIRE(mp.GetSize() == 0);
}


TEST_CASE("rehash map", "[map]") {

    Map<double, U64> mp(2, 1.f);

    REQUIRE(mp.GetNumBuckets() == 2);

    mp[2.3] = 23;
    mp[3.4] = 34;
    mp[4.5] = 45;

    REQUIRE(mp.GetNumBuckets() == 4);
}