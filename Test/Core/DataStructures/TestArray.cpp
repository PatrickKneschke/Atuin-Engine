
#include <catch2/catch.hpp>

#include "Core/DataStructures/Array.h"


using namespace Atuin;


TEST_CASE("create an empty array", "[array]") {

	Array<int> arr;

    REQUIRE(arr.GetCapacity() == 0);
    REQUIRE(arr.GetSize() == 0);
}

TEST_CASE("create an sized array", "[array]") {

	Array<int> arr(5);

    REQUIRE(arr.GetCapacity() == 5);
    REQUIRE(arr.GetSize() == 0);
}

TEST_CASE("create a sized array with default value", "[array]") {

	Array<int> arr(5, 0);

    REQUIRE(arr.GetCapacity() == 5);
    REQUIRE(arr.GetSize() == 5);
}

TEST_CASE("create an array from initializer list", "[array]") {

	Array<int> arr = {1,2,3};

    REQUIRE(arr.GetCapacity() == 3);
    REQUIRE(arr.GetSize() == 3);
}

TEST_CASE("create an array copy", "[array]") {

	Array<int> arr1 = {1,2,3};
    Array<int> arr2(arr1);

    REQUIRE(arr2.GetCapacity() == arr1.GetCapacity());
    REQUIRE(arr2.GetSize() == arr2.GetSize());
    REQUIRE(arr1.Data() != arr2.Data());
}

TEST_CASE("move an array", "[array]") {

	Array<int> arr1 = {1,2,3};
    int *data1 = arr1.Data();
    Array<int> arr2(std::move(arr1));

    REQUIRE(arr1.GetCapacity() == 0);
    REQUIRE(arr1.GetSize() == 0);
    REQUIRE(arr1.Data() == nullptr);
    REQUIRE(arr2.GetCapacity() == 3);
    REQUIRE(arr2.GetSize() == 3);
    REQUIRE(arr2.Data() == data1);
}

TEST_CASE("copy assign an array", "[array]") {

	Array<int> arr1 = {1,2,3};
    Array<int> arr2 = arr1;

    REQUIRE(arr2.GetCapacity() == arr1.GetCapacity());
    REQUIRE(arr2.GetSize() == arr2.GetSize());
    REQUIRE(arr1.Data() != arr2.Data());
}

TEST_CASE("move assign an array", "[array]") {

	Array<int> arr1 = {1,2,3};
    int *data1 = arr1.Data();
    Array<int> arr2 = std::move(arr1);

    REQUIRE(arr1.GetCapacity() == 0);
    REQUIRE(arr1.GetSize() == 0);
    REQUIRE(arr1.Data() == nullptr);
    REQUIRE(arr2.GetCapacity() == 3);
    REQUIRE(arr2.GetSize() == 3);
    REQUIRE(arr2.Data() == data1);
}

TEST_CASE("destroy an array", "[array]") {

    Array<int> arr = {1,2,3};
    arr.~Array();

    REQUIRE(arr.GetCapacity() == 0);
    REQUIRE(arr.GetSize() == 0);
    REQUIRE(arr.Data() == nullptr);
}

TEST_CASE("clear an array", "[array]") {

    Array<int> arr = {1,2,3};
    arr.Clear();

    REQUIRE(arr.GetCapacity() == 3);
    REQUIRE(arr.GetSize() == 0);
    REQUIRE(arr.Data() != nullptr);
}

TEST_CASE("reserve array space", "[array]") {
    
    Array<int> arr = {1,2,3};

    SECTION("reserve less space than array capacity")
    {
        arr.Reserve(2);

        REQUIRE(arr.GetCapacity() == 3);
        REQUIRE(arr.GetSize() == 3);
    }
    SECTION("reserve more space than array capacity")
    {
        arr.Reserve(5);

        REQUIRE(arr.GetCapacity() == 5);
        REQUIRE(arr.GetSize() == 3);
    }
}

TEST_CASE("resize an array", "[array]") {
    
    Array<int> arr = {1,2,3};

    SECTION("resize to make it larger")
    {
        arr.Resize(5);

        REQUIRE(arr.GetCapacity() == 5);
        REQUIRE(arr.GetSize() == 5);
    }
    SECTION("resize to make it smaller")
    { 
        arr.Resize(2);

        REQUIRE(arr.GetCapacity() == 3);
        REQUIRE(arr.GetSize() == 2);
    }
}

TEST_CASE("push back", "[array]") {
    
    Array<int> arr(1);
    SECTION("push back with enough space in array")
    {
        arr.PushBack(1);

        REQUIRE(arr.GetCapacity() == 1);
        REQUIRE(arr.GetSize() == 1);
        REQUIRE(arr.Back() == 1);
    }
    SECTION("push back causing array to reserve more space")
    {
        arr.PushBack(2);
        int i = 3;
        arr.PushBack(std::move(i));

        REQUIRE(arr.GetCapacity() == 2);
        REQUIRE(arr.GetSize() == 2);
        REQUIRE(arr.Back() == 3);
    }
}

TEST_CASE("pop back", "[array]") {

    Array<int> arr = {1,2,3};
    arr.PopBack();

    REQUIRE(arr.GetCapacity() == 3);
    REQUIRE(arr.GetSize() == 2);   
    REQUIRE(arr.Back() == 2);
}

TEST_CASE("emplace back", "[array]") {
    
    struct A {
        int i;
        float f;
        A(int i_, float f_) : i{i_}, f{f_} {}
    };

    Array<A> arr(3);
    arr.EmplaceBack(1, 2.34f);

    REQUIRE(arr.GetCapacity() == 3);
    REQUIRE(arr.GetSize() == 1);      
    REQUIRE(arr.Back().i == 1);
    REQUIRE(arr.Back().f == 2.34f);
}