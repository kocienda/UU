//
// array_test.cpp
//
#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

// ================================================================================================
// helpers

template <typename T>
static void assign_array_form(ArrayForm<T> &dst, const ArrayForm<T> &src) {
    dst = src;
}

struct Trivial {
    int foo;
    double bar;
};

struct NonTrivial {
    NonTrivial() {}
    ~NonTrivial() {}
    void *baz;
};

// ================================================================================================

TEST_CASE("array add int", "[array]" ) {
    Array<int, 4> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back(i);
    }
    for (int i = 10; i < 20; i++) {
        a1.emplace_back(i);
    }

    REQUIRE(a1.size() == 20);
    REQUIRE(a1.size_in_bytes() == 20 * sizeof(int));
}

TEST_CASE("array add char", "[array]" ) {
    Array<char> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back(i);
    }
    for (int i = 10; i < 20; i++) {
        a1.emplace_back(i);
    }

    REQUIRE(a1.size() == 20);
    REQUIRE(a1.size_in_bytes() == 20 * sizeof(char));
}

TEST_CASE("array add trivial", "[array]" ) {
    Array<Trivial> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back(Trivial());
    }
    for (int i = 10; i < 20; i++) {
        a1.emplace_back(Trivial());
    }

    REQUIRE(a1.size() == 20);
    REQUIRE(a1.size_in_bytes() == 20 * sizeof(Trivial));
}

TEST_CASE("array add nontrivial", "[array]" ) {
    Array<NonTrivial> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back(NonTrivial());
    }
    for (int i = 10; i < 20; i++) {
        a1.emplace_back(NonTrivial());
    }

    REQUIRE(a1.size() == 20);
    REQUIRE(a1.size_in_bytes() == 20 * sizeof(NonTrivial));
}

TEST_CASE("array swap int", "[array]" ) {
    Array<int> a1;
    Array<int> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back(i);
    }

    std::swap(a1, a2);

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
    REQUIRE(a2.size() == 10);
    REQUIRE(a2.size_in_bytes() == 10 * sizeof(int));
}

TEST_CASE("array swap char", "[array]" ) {
    Array<char> a1;
    Array<char> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back('0' + i);
    }

    std::swap(a1, a2);

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
    REQUIRE(a2.size() == 10);
    REQUIRE(a2.size_in_bytes() == 10 * sizeof(char));
}

TEST_CASE("array swap trivial", "[array]" ) {
    Array<Trivial> a1;
    Array<Trivial> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back(Trivial());
    }

    std::swap(a1, a2);

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
    REQUIRE(a2.size() == 10);
    REQUIRE(a2.size_in_bytes() == 10 * sizeof(Trivial));
}

TEST_CASE("array swap nontrivial", "[array]" ) {
    Array<NonTrivial> a1;
    Array<NonTrivial> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back(NonTrivial());
    }

    std::swap(a1, a2);

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
    REQUIRE(a2.size() == 10);
    REQUIRE(a2.size_in_bytes() == 10 * sizeof(NonTrivial));
}

TEST_CASE("array assign and clear int", "[array]" ) {
    Array<int> a1;
    Array<int> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back(i);
    }

    a2 = a1;
    a1.clear();

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
    REQUIRE(a2.size() == 10);
    REQUIRE(a2.size_in_bytes() == 10 * sizeof(int));
}

TEST_CASE("array assign and clear char", "[array]" ) {
    Array<char> a1;
    Array<char> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back('0' + i);
    }

    a2 = a1;
    a1.clear();

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
    REQUIRE(a2.size() == 10);
    REQUIRE(a2.size_in_bytes() == 10 * sizeof(char));
}

TEST_CASE("array assign and clear trivial", "[array]" ) {
    Array<Trivial> a1;
    Array<Trivial> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back(Trivial());
    }

    a2 = a1;
    a1.clear();

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
    REQUIRE(a2.size() == 10);
    REQUIRE(a2.size_in_bytes() == 10 * sizeof(Trivial));
}

TEST_CASE("array assign and clear nontrivial", "[array]" ) {
    Array<NonTrivial> a1;
    Array<NonTrivial> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back(NonTrivial());
    }

    a2 = a1;
    a1.clear();

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
    REQUIRE(a2.size() == 10);
    REQUIRE(a2.size_in_bytes() == 10 * sizeof(NonTrivial));
}

TEST_CASE("array assign form int", "[array]" ) {
    Array<int, 4> a1;
    Array<int, 2> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back(i);
    }

    assign_array_form(a2, a1);
    a1.clear();

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
    REQUIRE(a2.size() == 10);
    REQUIRE(a2.size_in_bytes() == 10 * sizeof(int));
}

TEST_CASE("array assign form char", "[array]" ) {
    Array<char, 4> a1;
    Array<char, 2> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back('0' + i);
    }

    assign_array_form(a2, a1);
    a1.clear();

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
    REQUIRE(a2.size() == 10);
    REQUIRE(a2.size_in_bytes() == 10 * sizeof(char));
}

TEST_CASE("array assign form trivial", "[array]" ) {
    Array<Trivial, 4> a1;
    Array<Trivial, 2> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back(Trivial());
    }

    assign_array_form(a2, a1);
    a1.clear();

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
    REQUIRE(a2.size() == 10);
    REQUIRE(a2.size_in_bytes() == 10 * sizeof(Trivial));
}

TEST_CASE("array assign form nontrivial", "[array]" ) {
    Array<NonTrivial, 4> a1;
    Array<NonTrivial, 2> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back(NonTrivial());
    }

    assign_array_form(a2, a1);
    a1.clear();

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
    REQUIRE(a2.size() == 10);
    REQUIRE(a2.size_in_bytes() == 10 * sizeof(NonTrivial));
}

