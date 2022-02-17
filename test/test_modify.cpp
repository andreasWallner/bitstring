#include "bitstring/bit_array.hpp"
#include "bitstring/exceptions.hpp"
#include "util.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("appending to bit array") {
  GIVEN("a bit array") {
    auto dut = bitstring::bit_array("0b1011010");
    WHEN("indexing bits") {
      THEN("value at [0] must be correct") { REQUIRE(dut[0] == 1); }
      THEN("value at [1] must be correct") { REQUIRE(dut[1] == 0); }
      THEN("value at [7] must be correct") { REQUIRE(dut[7] == 0); }
    }
    WHEN("appending") {
      dut.append("0b101101001");
      THEN("data must be appended correctly") {
        REQUIRE(dut == bitstring::bit_array("0b1011010101101001"));
      }
    }
  }
}
