
#include <catch2/catch.hpp>

#include "Core/DataStructures/Map.h"


using namespace Atuin;


TEST_CASE("create an empty map", "[map]") {

	Map<int, int> mp;
    Map<float, int> mp2(54, 1.2f);

    REQUIRE(mp.GetSize() == 0);
    REQUIRE(mp2.GetSize() == 0);
}


TEST_CASE("copy a map", "[map]") {

	Map<std::string, int> mp;
    mp["first"] = 1;

    REQUIRE(mp.GetSize() == 1);
    REQUIRE(mp["first"] == 1);

    Map<std::string, int> mp2(mp);

    REQUIRE(mp2.GetSize() == 1);
    // REQUIRE(mp2["first"] == 1);
    // REQUIRE(mp2["second"] == 1);

    // Map<std::string, int> mp3(std::move(mp));

    // REQUIRE(mp3.GetSize() == 1);
    // REQUIRE(mp3["first"] == 1);
    // REQUIRE(mp3.At("first") == 1);
    // REQUIRE(mp.GetSize() == 0);
    // REQUIRE_THROWS(mp.At("first") == 1);
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

    REQUIRE(mp.GetSize() == 3);
    REQUIRE_NOTHROW( mp.Insert(4, 16) );
    REQUIRE(mp.GetSize() == 4);
    REQUIRE_NOTHROW( mp.Erase(2) );
    REQUIRE(mp.GetSize() == 3);

    mp[1] = 1.f;
    REQUIRE(mp[1] == 1.f);

    REQUIRE_NOTHROW( mp.Clear() );
    REQUIRE(mp.GetSize() == 0);
}


TEST_CASE("rehash map", "[map]") {

    Map<double, U64> mp(2, 1.f);

    REQUIRE(mp.GetNumBuckets() == 2);

    // mp[2.3] = 23;
    // mp[3.4] = 34;
    // mp[4.5] = 45;

    // REQUIRE(mp.GetNumBuckets() == 4);
}