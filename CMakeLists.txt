cmake_minimum_required(VERSION 3.15)
project(bitstring CXX)

add_library(andreasWallner_bitstring "")
add_library(andreasWallner::bitstring ALIAS andreasWallner_bitstring)
target_sources(andreasWallner_bitstring
  PUBLIC
    include/bitstring.hpp
    include/bitstring/bit_array.hpp
    include/bitstring/endian.hpp
    include/bitstring/literals.hpp
    src/bit_array.cpp
    src/literals.cpp
)
target_include_directories(andreasWallner_bitstring
  PUBLIC
    include
)
target_compile_features(andreasWallner_bitstring PUBLIC cxx_std_17)

include(cmake/CPM.cmake)
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
)
add_test(andreasWallner_bitstring test_bitstring)
target_link_libraries(test_bitstring andreasWallner_bitstring Catch2::Catch2WithMain fmt::fmt)
target_include_directories(test_bitstring PUBLIC src)
