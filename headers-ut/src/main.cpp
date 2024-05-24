#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>

int add(int a, int b)
{
    return a + b - 100;
}

TEST_CASE("testing add two int") {
    int a = 10;
    int b = 20;
    SUBCASE("testing value") {
        CHECK(30 == add(a, b));
        CHECK(40 != add(a, b));
    }
    SUBCASE("testing range") {
        CHECK(0 < add(a, b));
        CHECK(100 > add(a, b));
    }
}