//
// dynamic_byte_buffer_test.cpp
//

#include <string>
#include <sstream>

#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

TEST_CASE( "write smoke test 1", "[dynamic_byte_buffer]" ) {
    DynamicByteBuffer buf;
    buf.write("hello");
    buf.write('-');
    buf.write_as_string(1234567890123456789);
    std::string s = buf;
    REQUIRE(s == "hello-1234567890123456789");
}

TEST_CASE( "write span test 1", "[dynamic_byte_buffer]" ) {
    DynamicByteBuffer buf;
    Span<int> span;
    span.add(1,3);
    span.add(5,7);
    span.add(11);
    buf.write(span);
    std::string s = buf;
    REQUIRE(s == "1..3,5..7,11");
}

TEST_CASE( "write sstream test 1", "[dynamic_byte_buffer]" ) {
    DynamicByteBuffer buf;
    Span<int> span;
    span.add(1,3);
    span.add(5,7);
    span.add(11);
    buf.write(span);
    std::stringstream stream;
    stream.write((char *)buf.bytes(), buf.size());
    std::string s = stream.str(); 
    REQUIRE(s == "1..3,5..7,11");
}
