#include <catch2/catch.hpp>

#include "Core/DataStructures/Queue.h"


using namespace Atuin;


TEST_CASE("create an empty queue", "[queue]") {

    SECTION("queue of initila size zero")
    {
        Queue<int> q1;

        REQUIRE(q1.GetSize() == 0);
    }
    SECTION("queue with initil size")
    {
        Queue<int> q2(5);

        REQUIRE(q2.GetSize() == 0);
    }
}


TEST_CASE("create a queue from initializer list", "[queue]") {

    Queue<int> q2({1,2,3});

    REQUIRE(q2.GetSize() == 3);
    REQUIRE(q2.Front() == 1);
    REQUIRE(q2.Back() == 3);
}


TEST_CASE("create a queue from array", "[queue]") {

    Array<int> arr{1,2,3};
    Queue<int> q2(arr);

    REQUIRE(q2.GetSize() == 3);
    REQUIRE(q2.Front() == 1);
    REQUIRE(q2.Back() == 3);
}


TEST_CASE("copy a queue", "[queue]") {

    Queue<int> q1{1,2,3};

    SECTION("regular copy")
    {
        Queue<int> q2(q1);

        REQUIRE(q2.GetSize() == q1.GetSize());
    }
    SECTION("move copy")
    {
        Queue<int> q3(std::move(q1));

        REQUIRE(q3.GetSize() == 3);
    }
}


TEST_CASE("assign a queue", "[queue]") {

    Queue<int> q1{1,2,3};

    SECTION("copy assign")
    {
        Queue<int> q2 = q1;

        REQUIRE(q2.GetSize() == q1.GetSize());
    }
    SECTION("move assign")
    {
        Queue<int> q3 = std::move(q1);

        REQUIRE(q3.GetSize() == 3);
    }
    SECTION("assign initilizer list")
    {
        Queue<int> q4 = {1,2,3};

        REQUIRE(q4.GetSize() == 3);
    }
}


TEST_CASE("modify a queue", "[queue]") {

    Queue<int> q = {1,2,3};

    SECTION("push")
    {
        q.Push(4);
        REQUIRE(q.Back() == 4);
    }
    SECTION("pop")
    {
        q.Pop();
        REQUIRE(q.Front() == 2);
    }
    SECTION("clear")
    {
        q.Clear();
        REQUIRE(q.GetSize() == 0);
    }
}


TEST_CASE("access queue elements", "[queue]") {

    Queue<int> q = {1,2};

    SECTION("front")
    {
        REQUIRE(q.Front() == 1);
    }
    SECTION("back")
    {
        REQUIRE(q.Back() == 2);
    }
    SECTION("throws if emoty")
    {
        q.Clear();
        REQUIRE_THROWS(q.Front());
        REQUIRE_THROWS(q.Back());
    }
}