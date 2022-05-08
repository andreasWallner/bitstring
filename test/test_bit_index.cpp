#include "bitstring/bit_array.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("log2ceil") {
  REQUIRE(bitstring::detail::log2ceil(0) == 0);
  REQUIRE(bitstring::detail::log2ceil(1) == 1);
  REQUIRE(bitstring::detail::log2ceil(2) == 2);
  REQUIRE(bitstring::detail::log2ceil(3) == 2);
  REQUIRE(bitstring::detail::log2ceil(4) == 3);
}

TEST_CASE("bit_index uint32_t 0") {
  auto idx = bitstring::detail::bit_index(0);

  REQUIRE(idx.unit() == 0);
  REQUIRE(idx.bit_offset() == 0);
  REQUIRE(idx.bit_mask() == 0x00000001);
}

TEST_CASE("bit_index uint32_t 1") {
  auto idx = bitstring::detail::bit_index(1);

  REQUIRE(idx.unit() == 0);
  REQUIRE(idx.bit_offset() == 1);
  REQUIRE(idx.bit_mask() == 0x0000002);
}

TEST_CASE("bit_index uint32_t 31") {
  auto idx = bitstring::detail::bit_index(31);

  REQUIRE(idx.unit() == 0);
  REQUIRE(idx.bit_offset() == 31);
  REQUIRE(idx.bit_mask() == 0x80000000);
}

TEST_CASE("bit_index uint32_t 32") {
  auto idx = bitstring::detail::bit_index(32);

  REQUIRE(idx.unit() == 1);
  REQUIRE(idx.bit_offset() == 0);
  REQUIRE(idx.bit_mask() == 0x00000001);
}

TEST_CASE("bit_index uint32_t 33") {
  auto idx = bitstring::detail::bit_index(33);

  REQUIRE(idx.unit() == 1);
  REQUIRE(idx.bit_offset() == 1);
  REQUIRE(idx.bit_mask() == 0x00000002);
}
