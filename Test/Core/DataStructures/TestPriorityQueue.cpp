
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
        PriorityQueue<int, Math::Greater<int>> pq4{2,3,1};

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

TEST_CASE("use custom type", "[priority queue]") {

    struct A {

        int i1 = 0, i2 = 0;

        bool operator< (const A &rhs) const {

            return i1*i2 < rhs.i1*rhs.i2;
        }
        bool operator> (const A &rhs) const {

            return i1*i2 > rhs.i1*rhs.i2;
        }
        bool operator== (const A &rhs) const{

            return i1 == rhs.i1 && i2 == rhs.i2;
        }
    };

    PriorityQueue<A, Math::Greater<A>> pq;
    pq.Push({-1, 1});
    pq.Push({-1, -1});

    REQUIRE(pq.Top() == A{-1,-1});
}

TEST_CASE("use custom comparator", "[priority queue]") {

    SECTION("comparator class")
    {
        struct EvenOddLess {

            bool operator() (const int &a, const int &b) {

                if (a % 2 == 0 && b % 2 != 0)
                {
                    return true;
                }
                else if (a % 2 != 0 && b % 2 == 0)
                {
                    return false;
                }
                else
                {
                    return a < b;
                }
            }
        };

        PriorityQueue<int, EvenOddLess> pq1;

        pq1.Push(3);
        pq1.Push(4);
        REQUIRE(pq1.Top() == 4);

        pq1.Push(1);
        pq1.Push(2);
        REQUIRE(pq1.Top() == 2);

        pq1.Pop();
        pq1.Pop();
        REQUIRE(pq1.Top() == 1);
    }
    SECTION("use lambda expression") 
    {
        auto comp = [](const float &a, const float &b) {

            return std::abs(10.f - a) < std::abs(10.f - b);
        };

        PriorityQueue<float, decltype(comp)> pq2 = {-2.5f, 5.6f, 9.3f, 10.7f, 16.f};
        
        REQUIRE(pq2.Top() == 9.3f);
    }
}