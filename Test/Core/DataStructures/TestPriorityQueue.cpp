
#include <catch2/catch.hpp>

#include "Core/DataStructures/PriorityQueue.h"


using namespace Atuin;


TEST_CASE("create a priority queue", "[priority queue]") {

    SECTION("set initial size")
    {
        PriorityQueue<int> pq1(5);

        REQUIRE(pq1.GetSize() == 0);
    }
    SECTION("from initializer list")
    {
        PriorityQueue<int> pq2{2,3,1};

        REQUIRE(pq2.GetSize() == 3);
        for (int i=1; i<=3; i++)
        {
            REQUIRE(pq2.Top() == i);
            pq2.Pop();
        }
    }
    SECTION("from array")
    {
        Array<int> arr{2,3,1};
        PriorityQueue<int> pq3{arr};

        REQUIRE(pq3.GetSize() == arr.GetSize());
        for (int i=1; i<=3; i++)
        {
            REQUIRE(pq3.Top() == i);
            pq3.Pop();
        }
    }    
    SECTION("use different comparator")
    {
        PriorityQueue<int, Math::Greater> pq4{2,3,1};

        REQUIRE(pq4.GetSize() == 3);
        for (int i=3; i>=1; i--)
        {
            REQUIRE(pq4.Top() == i);
            pq4.Pop();
        }
    }
}

TEST_CASE("modify priority queue", "[priority queue]") {

    PriorityQueue<int> pq(3);

    pq.Push(2);
    pq.Emplace(1);
    REQUIRE(pq.Top() == 1);

    pq.Pop();
    REQUIRE(pq.Top() == 2);

    pq.Clear();
    REQUIRE_THROWS(pq.Top());


}