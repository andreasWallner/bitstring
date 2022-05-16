cmake_minimum_required(VERSION 3.15)

if(NOT DEFINED PROJECT_NAME)
  set(BITSTRING_ROOT_PROJECT ON)
endif()

project(bitstring CXX)

option(BITSTRING_ENABLE_TESTS "Build & register bitstring unittests" ${BITSTRING_ROOT_PROJECT})
if(BITSTRING_ROOT_PROJECT)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

include(cmake/CompilerWarnings.cmake)
include(cmake/DefaultSettings.cmake)
include(cmake/StaticAnalyzers.cmake)

add_library(bitstring "")
add_library(bitstring::bitstring ALIAS bitstring)
target_sources(bitstring
  PRIVATE
    include/bitstring.hpp
    include/bitstring/bit_array.hpp
    include/bitstring/endian.hpp
    include/bitstring/literals.hpp
    src/bit_array.cpp
    src/literals.cpp
)
target_include_directories(bitstring
  PRIVATE
    include
)
target_include_directories(bitstring
  SYSTEM INTERFACE
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features(bitstring PUBLIC cxx_std_17)
target_set_warnings(bitstring)
target_enable_clang_tidy(bitstring BITSTRING_CLANG_TIDY)

if(BITSTRING_ENABLE_TESTS)
  include(cmake/CPM.cmake)
  set(FETCHCONTENT_UPDATES_DISCONNECTED  TRUE)
  CPMAddPackage("gh:catchorg/Catch2@3.0.0-preview4")
  CPMAddPackage("gh:fmtlib/fmt#8.1.1")

  enable_testing()
  add_executable(test_bitstring
    test/util.cpp
    test/test_init.cpp
    test/test_format.cpp
    test/test_comparison.cpp
    test/test_endian.cpp
    test/test_modify.cpp
    test/test_operators.cpp

    test/test_bit_index.cpp
  )
  target_include_directories(test_bitstring PUBLIC src)
  target_link_libraries(test_bitstring bitstring Catch2::Catch2WithMain fmt::fmt)
  target_set_warnings(test_bitstring)
  target_enable_clang_tidy(test_bitstring BITSTRING_CLANG_TIDY)
  add_test(bitstring test_bitstring)
endif()
