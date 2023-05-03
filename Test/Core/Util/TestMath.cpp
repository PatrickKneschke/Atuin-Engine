
#include <catch2/catch.hpp>

#include "Core/Util/Math.h"


using namespace Atuin;


TEST_CASE("IsPowerOfTwo", "[math]") {

    REQUIRE( Math::IsPowerOfTwo(0) == false );
    REQUIRE( Math::IsPowerOfTwo(1) == true );
    REQUIRE( Math::IsPowerOfTwo(256) == true );
    REQUIRE( Math::IsPowerOfTwo(((U64)1<<63)) == true );
}

TEST_CASE("NextPowerOfTwo", "[math]") {

    REQUIRE( Math::NextPowerOfTwo(0) == 1 );
    REQUIRE( Math::NextPowerOfTwo(1) == 2 );
    REQUIRE( Math::NextPowerOfTwo(1643) == 2048 );
    SECTION("prevent overflow for very large inputs")
    {
        U64 veryLargeNumber = ((U64)3<<62);
        REQUIRE( Math::NextPowerOfTwo(veryLargeNumber) == veryLargeNumber );
    }
}

TEST_CASE("PrevPowerOfTwo", "[math]") {

    SECTION("prevent underflow")
    {
        REQUIRE( Math::PrevPowerOfTwo(0) == 0 );
        REQUIRE( Math::PrevPowerOfTwo(1) == 1 );
    }
    REQUIRE( Math::PrevPowerOfTwo(1643) == 1024 );
}

TEST_CASE("ClosestPowerOfTwo", "[math]") {

    REQUIRE( Math::ClosestPowerOfTwo(256) == 256 );
    REQUIRE( Math::ClosestPowerOfTwo(13) == 16 );
    REQUIRE( Math::ClosestPowerOfTwo(37) == 32 );
    SECTION("return smaller power of two in case of a tie")
    {
        REQUIRE( Math::ClosestPowerOfTwo(96) == 64 );
    }
}

TEST_CASE("ModuloPowerOfTwo", "[math]") {

    SECTION("zero modulo anything is zero")
    {
        REQUIRE( Math::ModuloPowerOfTwo(0, 32) == 0 );
    }
    SECTION("anything modulo one is zero")
    {
        REQUIRE( Math::ModuloPowerOfTwo(57, 1) == 0 );
    }
    REQUIRE( Math::ModuloPowerOfTwo(6, 2) == 0 );
    REQUIRE( Math::ModuloPowerOfTwo(7, 2) == 1 );
    REQUIRE( Math::ModuloPowerOfTwo(57, 16) == 9 );
    REQUIRE( Math::ModuloPowerOfTwo(64, 16) == 0 );
    REQUIRE( Math::ModuloPowerOfTwo(64, 64) == 0 );
}