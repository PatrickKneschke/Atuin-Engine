
#include <catch2/catch.hpp>

#include "Core/DataStructures/Array.h"


using namespace Atuin;


TEST_CASE("create empty", "[constructor]") {

	Array<int> arr;

    REQUIRE(arr.GetCapacity() == 0);
    REQUIRE(arr.GetSize() == 0);
}

TEST_CASE("create at size", "[constructor]") {

	Array<int> arr(5);

    REQUIRE(arr.GetCapacity() == 5);
    REQUIRE(arr.GetSize() == 0);
}

TEST_CASE("create with default value", "[constructor]") {

	Array<int> arr(5, 0);

    REQUIRE(arr.GetCapacity() == 5);
    REQUIRE(arr.GetSize() == 5);
}

TEST_CASE("initializer list", "[constructor]") {

	Array<int> arr = {1,2,3};

    REQUIRE(arr.GetCapacity() == 3);
    REQUIRE(arr.GetSize() == 3);
}

TEST_CASE("copy", "[constructor]") {

	Array<int> arr1 = {1,2,3};
    Array<int> arr2(arr1);

    REQUIRE(arr2.GetCapacity() == arr1.GetCapacity());
    REQUIRE(arr2.GetSize() == arr2.GetSize());
    REQUIRE(arr1.Data() != arr2.Data());
}

TEST_CASE("move", "[constructor]") {

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

TEST_CASE("copy assigment", "[assignment]") {

	Array<int> arr1 = {1,2,3};
    Array<int> arr2 = arr1;

    REQUIRE(arr2.GetCapacity() == arr1.GetCapacity());
    REQUIRE(arr2.GetSize() == arr2.GetSize());
    REQUIRE(arr1.Data() != arr2.Data());
}

TEST_CASE("move assignment", "[assignment]") {

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

TEST_CASE("destructor", "[destructor]") {

    Array<int> arr = {1,2,3};
    arr.~Array();

    REQUIRE(arr.GetCapacity() == 0);
    REQUIRE(arr.GetSize() == 0);
    REQUIRE(arr.Data() == nullptr);
}

TEST_CASE("clear", "[modify]") {

    Array<int> arr = {1,2,3};
    arr.Clear();

    REQUIRE(arr.GetCapacity() == 3);
    REQUIRE(arr.GetSize() == 0);
    REQUIRE(arr.Data() != nullptr);
}

TEST_CASE("reserve", "[modify]") {
    
    Array<int> arr = {1,2,3};
    arr.Reserve(2);

    REQUIRE(arr.GetCapacity() == 3);
    REQUIRE(arr.GetSize() == 3);

    arr.Reserve(5);

    REQUIRE(arr.GetCapacity() == 5);
    REQUIRE(arr.GetSize() == 3);
}

// TEST_CASE("resize", "[modify]") {
    
//     Array<int> arr = {1,2,3};
//     arr.Resize(5);

//     REQUIRE(arr.GetCapacity() == 5);
//     REQUIRE(arr.GetSize() == 5);

//     arr.Resize(2);

//     REQUIRE(arr.GetCapacity() == 5);
//     REQUIRE(arr.GetSize() == 2);
// }

TEST_CASE("push", "[modify]") {
    
    Array<int> arr;
    arr.PushBack(1);

    REQUIRE(arr.GetCapacity() == 1);
    REQUIRE(arr.GetSize() == 1);
    REQUIRE(arr.Back() == 1);

    arr.PushBack(2);
    int i = 3;
    arr.PushBack(std::move(i));

    REQUIRE(arr.GetCapacity() == 4);
    REQUIRE(arr.GetSize() == 3);
    REQUIRE(arr.Back() == 3);
}

TEST_CASE("pop", "[modify]") {

    Array<int> arr = {1,2,3};
    arr.PopBack();

    REQUIRE(arr.GetCapacity() == 3);
    REQUIRE(arr.GetSize() == 2);   
    REQUIRE(arr.Back() == 2);
}

TEST_CASE("emplace", "[modify]") {
    
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