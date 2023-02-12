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

    friend bool operator==(const Trivial &a, const Trivial &b) {
        return a.foo == b.foo && a.bar == b.bar;
    }

};

struct NonTrivial {
    NonTrivial() {}
    NonTrivial(void *p) : baz(p) {}
    ~NonTrivial() {}
    void *baz;

    friend bool operator==(const NonTrivial &a, const NonTrivial &b) {
        return a.baz == b.baz;
    }
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

TEST_CASE("array pop until empty int", "[array]" ) {
    Array<int> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back(i);
    }

    while (a1.not_empty()) {
        a1.pop_back();
    }

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
}

TEST_CASE("array pop until empty char", "[array]" ) {
    Array<char> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back('0' + i);
    }

    while (a1.not_empty()) {
        a1.pop_back();
    }

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
}

TEST_CASE("array pop until empty trivial", "[array]" ) {
    Array<Trivial> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back(Trivial());
    }

    while (a1.not_empty()) {
        a1.pop_back();
    }

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
}

TEST_CASE("array pop until empty nontrivial", "[array]" ) {
    Array<NonTrivial> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back(NonTrivial());
    }

    while (a1.not_empty()) {
        a1.pop_back();
    }

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
}

TEST_CASE("array pop_back_n to empty int", "[array]" ) {
    Array<int> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back(i);
    }

    a1.pop_back_n(a1.size());

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
}

TEST_CASE("array pop_back_n to empty char", "[array]" ) {
    Array<char> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back('0' + i);
    }

    a1.pop_back_n(a1.size());

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
}

TEST_CASE("array pop_back_n to empty trivial", "[array]" ) {
    Array<Trivial> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back(Trivial());
    }

    a1.pop_back_n(a1.size());

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
}

TEST_CASE("array pop_back_n to empty nontrivial", "[array]" ) {
    Array<NonTrivial> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back(NonTrivial());
    }

    a1.pop_back_n(a1.size());

    REQUIRE(a1.size() == 0);
    REQUIRE(a1.size_in_bytes() == 0);
}

TEST_CASE("array insert int", "[array]" ) {
    Array<int> a1;
    Array<int> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back(i);
    }

    a2.insert(a2.end(), a1.begin(), a1.end());

    REQUIRE(a1.size() == 10);
    REQUIRE(a1.size_in_bytes() == 10 * sizeof(int));
    REQUIRE(a1.size() == a2.size());
    REQUIRE(a1.size_in_bytes() == a2.size_in_bytes());
}

TEST_CASE("array insert char", "[array]" ) {
    Array<char> a1;
    Array<char> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back('0' + i);
    }

    a2.insert(a2.end(), a1.begin(), a1.end());

    REQUIRE(a1.size() == 10);
    REQUIRE(a1.size_in_bytes() == 10 * sizeof(char));
    REQUIRE(a1.size() == a2.size());
    REQUIRE(a1.size_in_bytes() == a2.size_in_bytes());
}

TEST_CASE("array insert trivial", "[array]" ) {
    Array<Trivial> a1;
    Array<Trivial> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back(Trivial());
    }

    a2.insert(a2.end(), a1.begin(), a1.end());

    REQUIRE(a1.size() == 10);
    REQUIRE(a1.size_in_bytes() == 10 * sizeof(Trivial));
    REQUIRE(a1.size() == a2.size());
    REQUIRE(a1.size_in_bytes() == a2.size_in_bytes());
}

TEST_CASE("array insert nontrivial", "[array]" ) {
    Array<NonTrivial> a1;
    Array<NonTrivial> a2;

    for (int i = 0; i < 10; i++) {
        a1.push_back(NonTrivial());
    }

    a2.insert(a2.end(), a1.begin(), a1.end());

    REQUIRE(a1.size() == 10);
    REQUIRE(a1.size_in_bytes() == 10 * sizeof(NonTrivial));
    REQUIRE(a1.size() == a2.size());
    REQUIRE(a1.size_in_bytes() == a2.size_in_bytes());
}

TEST_CASE("array erase one int", "[array]" ) {
    Array<int> a1;

    for (int i = 0; i < 11; i++) {
        a1.push_back(i);
    }

    a1.erase(a1.begin() + 5);

    REQUIRE(a1.size() == 10);
    REQUIRE(a1.size_in_bytes() == 10 * sizeof(int));
}

TEST_CASE("array erase one char", "[array]" ) {
    Array<char> a1;

    for (int i = 0; i < 11; i++) {
        a1.push_back('0' + i);
    }

    a1.erase(a1.begin() + 5);

    REQUIRE(a1.size() == 10);
    REQUIRE(a1.size_in_bytes() == 10 * sizeof(char));
}

TEST_CASE("array erase one trivial", "[array]" ) {
    Array<Trivial> a1;

    for (int i = 0; i < 11; i++) {
        a1.push_back(Trivial());
    }

    a1.erase(a1.begin() + 5);

    REQUIRE(a1.size() == 10);
    REQUIRE(a1.size_in_bytes() == 10 * sizeof(Trivial));
}

TEST_CASE("array erase one nontrivial", "[array]" ) {
    Array<NonTrivial> a1;

    for (int i = 0; i < 11; i++) {
        a1.push_back(NonTrivial());
    }

    a1.erase(a1.begin() + 5);

    REQUIRE(a1.size() == 10);
    REQUIRE(a1.size_in_bytes() == 10 * sizeof(NonTrivial));
}

TEST_CASE("array erase range of int", "[array]" ) {
    Array<int> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back(i);
    }

    a1.erase(a1.begin() + 5, a1.end());

    REQUIRE(a1.size() == 5);
    REQUIRE(a1.size_in_bytes() == 5 * sizeof(int));
    for (int i = 0; i < a1.size(); i++) {
        REQUIRE(a1[i] == i);
    }
}

TEST_CASE("array erase range of char", "[array]" ) {
    Array<char> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back('0' + i);
    }

    a1.erase(a1.begin() + 5, a1.end());

    REQUIRE(a1.size() == 5);
    REQUIRE(a1.size_in_bytes() == 5 * sizeof(char));
    for (int i = 0; i < a1.size(); i++) {
        REQUIRE(a1[i] == '0' + i);
    }
}

TEST_CASE("array erase range of trivial", "[array]" ) {
    Array<Trivial> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back({ i, double(i) });
    }

    a1.erase(a1.begin() + 5, a1.end());

    REQUIRE(a1.size() == 5);
    REQUIRE(a1.size_in_bytes() == 5 * sizeof(Trivial));
    for (int i = 0; i < a1.size(); i++) {
        REQUIRE(a1[i] == Trivial({ i, double(i) }));
    }
}

TEST_CASE("array erase range of nontrivial", "[array]" ) {
    Array<NonTrivial> a1;

    for (int i = 0; i < 10; i++) {
        a1.push_back(NonTrivial(reinterpret_cast<void *>(i)));
    }

    a1.erase(a1.begin() + 5, a1.end());

    REQUIRE(a1.size() == 5);
    REQUIRE(a1.size_in_bytes() == 5 * sizeof(NonTrivial));
    for (int i = 0; i < a1.size(); i++) {
        REQUIRE(a1[i] == NonTrivial(reinterpret_cast<void *>(i)));
    }
}

