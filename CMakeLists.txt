#
# CMakeLists.txt for UU
#
cmake_minimum_required(VERSION 3.0.0)

# Set the project name and version
project(UU VERSION 0.1.0)

set(CXX_VERSION_FLAGS "-std=c++17")
set(CXX_ARCH_FLAGS "-arch arm64")
set(CXX_WARNING_FLAGS "-Werror -Wno-trigraphs -Wno-missing-field-initializers -Wno-missing-prototypes -Werror=return-type -Wdocumentation \
-Wunreachable-code -Wno-non-virtual-dtor -Wno-overloaded-virtual -Wno-exit-time-destructors -Wno-missing-braces -Wparentheses -Wswitch \
-Wunused-function -Wno-unused-label -Wno-unused-parameter -Wunused-variable -Wunused-value -Wempty-body -Wuninitialized \
-Wconditional-uninitialized -Wno-unknown-pragmas -Wno-shadow -Wno-four-char-constants -Wno-conversion -Wconstant-conversion \
-Wint-conversion -Wbool-conversion -Wenum-conversion -Wno-float-conversion -Wnon-literal-null-conversion \
-Wshorten-64-to-32 -Wno-newline-eof -Wno-c++11-extensions -Wdeprecated-declarations -Winvalid-offsetof -Wno-sign-conversion \
-Winfinite-recursion -Wmove -Wcomma -Wstrict-prototypes -Wrange-loop-analysis -Wunguarded-availability -Wno-nullability-completeness \
-fvisibility-inlines-hidden -fasm-blocks -fstrict-aliasing -fno-common")
set(CMAKE_CXX_FLAGS "${CXX_VERSION_FLAGS} ${CXX_ARCH_FLAGS} ${CXX_WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG")

IF(DEFINED ENV{UDIR})
set(PREFIX $ENV{UDIR})
set(CMAKE_INSTALL_PREFIX ${PREFIX})
ELSE()
set(PREFIX /usr/local)
set(CMAKE_INSTALL_PREFIX ${PREFIX})
ENDIF()

set(CODE_DIR UU)

set(PUBLIC_HEADERS
  ${CODE_DIR}/UU.h
  ${CODE_DIR}/Assertions.h
  ${CODE_DIR}/AcquireReleaseGuard.h
  ${CODE_DIR}/Any.h
  ${CODE_DIR}/ByteReader.h
  ${CODE_DIR}/ByteWriter.h
  ${CODE_DIR}/CloseGuard.h
  ${CODE_DIR}/Compiler.h
  ${CODE_DIR}/FileLike.h
  ${CODE_DIR}/IteratorWrapper.h
  ${CODE_DIR}/MappedFile.h
  ${CODE_DIR}/MathLike.h
  ${CODE_DIR}/Platform.h
  ${CODE_DIR}/Range.h
  ${CODE_DIR}/SmallMap.h
  ${CODE_DIR}/SmallVector.h
  ${CODE_DIR}/Span.h
  ${CODE_DIR}/StackTrace.h
  ${CODE_DIR}/StaticByteBuffer.h
  ${CODE_DIR}/StringLike.h
  ${CODE_DIR}/TextRef.h
  ${CODE_DIR}/Types.h
  ${CODE_DIR}/UTF8.h
  ${CODE_DIR}/UnixLike.h
  ${CODE_DIR}/UUString.h
)

set(SOURCES
  ${CODE_DIR}/Assertions.cpp
  ${CODE_DIR}/FileLike.cpp
  ${CODE_DIR}/MappedFile.cpp
  ${CODE_DIR}/SmallVector.cpp
  ${CODE_DIR}/Span.cpp
  ${CODE_DIR}/StackTrace.cpp
  ${CODE_DIR}/StringLike.cpp
  ${CODE_DIR}/TextRef.cpp
  ${CODE_DIR}/UnixLike.cpp
  ${CODE_DIR}/UUString.cpp
)

# Add the library target
add_library(UU ${SOURCES} ${PUBLIC_HEADERS})

# Set the include directories for the library
target_include_directories(UU
  PUBLIC
    $<INSTALL_INTERFACE:include/UU>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Set the installation directory for the library
install(TARGETS UU
  EXPORT UUConfig
  ARCHIVE  DESTINATION lib
  LIBRARY  DESTINATION lib
  RUNTIME  DESTINATION bin
)

# Set the installation directory for the library headers
install(FILES ${PUBLIC_HEADERS} DESTINATION include/UU)

# Set the installation directory for the cmake configuration file
install(EXPORT UUConfig DESTINATION lib/cmake/UU)

# Set the export target for the library
export(TARGETS UU FILE UUConfig.cmake)


IF(DEFINED ENV{UU_UNIT_TEST})

# Unit tests
include(CTest)
enable_testing()
find_package(Catch2 3 REQUIRED)
include(Catch)

function(UU_TEST test_name)
add_executable(${test_name} test/${test_name}.cpp)
target_include_directories(${test_name} PRIVATE . /u/include)
target_link_directories(${test_name} PRIVATE build /u/lib)
target_link_libraries(${test_name} PRIVATE UU libCatch2.a libCatch2Main.a)
catch_discover_tests(${test_name})
endfunction()

UU_TEST(smoke_test)
UU_TEST(file_like_test)
UU_TEST(math_like_test)
UU_TEST(span_test)
UU_TEST(string_test)
UU_TEST(string_like_test)
UU_TEST(textref_test)
UU_TEST(unix_like_test)

ENDIF()

# To use:
# find_package(UU REQUIRED)
# # Add the include directories for the library
# target_include_directories(MyTarget PRIVATE ${UU_INCLUDE_DIRS})
# # Link the library to your target
# target_link_libraries(MyTarget UU)
