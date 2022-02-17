#include "bitstring/bit_array.hpp"
#include "util.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("comparing bit_arrays") {
  GIVEN("matching bit_arrays, multiple full storage_units") {
    auto a = bitstring::bit_array(
        std::vector<uint8_t>{0xaa, 0xbb, 0xcc, 0xdd, 0x11, 0x22, 0x33, 0x44});
    auto b = bitstring::bit_array(
        std::vector<uint8_t>{0xaa, 0xbb, 0xcc, 0xdd, 0x11, 0x22, 0x33, 0x44});
    WHEN("comparing") {
      THEN("comparison must yield they are equal") {
        REQUIRE(a.bin() == b.bin());
        REQUIRE(a == b);
        REQUIRE_FALSE(a != b);
      }
    }
  }
  GIVEN("bit_array with matching storage_units, different length") {
    auto a = bitstring::bit_array(
        0xaabbccddU, sizeof(bitstring::bit_array::storage_type) * 8);
    auto b = bitstring::bit_array(
        0xaabbccddU, sizeof(bitstring::bit_array::storage_type) * 8 - 8);
    WHEN("comparing") {
      THEN("comparison must yield they are equal") {
        REQUIRE(a != b);
        REQUIRE_FALSE(a == b);
      }
    }
  }
  GIVEN("two vector with length % 8 != 0") {
    auto a = bitstring::bit_array(0x0aU, 4);
    auto b = bitstring::bit_array(0xfaU, 4);
    WHEN("comparing") {
      THEN("comparison must yield they are equal") {
        REQUIRE(a == b);
        REQUIRE_FALSE(a != b);
      }
    }
  }
  GIVEN("single storage unit, msb differs") {
    static_assert(sizeof(bitstring::bit_array::storage_type) == sizeof(uint32_t));
    auto a = bitstring::bit_array(uint32_t{0x80000000U});
    auto b = bitstring::bit_array(uint32_t{0x00000000U});
    WHEN("comparing") {
      THEN("comparison must yield they differ") {
        REQUIRE_FALSE(a == b);
        REQUIRE(a != b);
      }
    }
  }
  GIVEN("single storage unit, lsb differs") {
    static_assert(sizeof(bitstring::bit_array::storage_type) == sizeof(uint32_t));
    auto a = bitstring::bit_array(uint32_t{0x00000000U});
    auto b = bitstring::bit_array(uint32_t{0x00000008U});
    WHEN("comparing") {
      THEN("comparison must yield they differ") {
        REQUIRE_FALSE(a == b);
        REQUIRE(a != b);
      }
    }
  }
}
