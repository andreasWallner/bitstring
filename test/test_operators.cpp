#include "bitstring/bit_array.hpp"
#include "bitstring/exceptions.hpp"
#include "util.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("multiplying bit arrays") {
  GIVEN("a bit array") {
    auto ba = bitstring::bit_array("0b1101");
    WHEN("multiplying left * 3") {
      auto dut = 3 * ba;
      THEN("result must be 3 x the original bit array") {
        REQUIRE(dut == bitstring::bit_array("0b110111011101"));
      }
    }
    WHEN("multiplying right * 3") {
      auto dut = ba * 3;
      THEN("result must be 3 x the original bit array") {
        REQUIRE(dut == bitstring::bit_array("0b110111011101"));
      }
    }
  }
}

SCENARIO("adding bit arrays") {
  GIVEN("two bit array") {
    auto ba1 = bitstring::bit_array("0b1001");
    auto ba2 = bitstring::bit_array("0b1110");
    WHEN("1 + 2") {
      auto dut = ba1 + ba2;
      THEN("result must be the concatenation") {
        REQUIRE(dut == bitstring::bit_array("0b10011110"));
      }
    }
    WHEN("2 + 1") {
      auto dut = ba2 + ba1;
      THEN("result must be the concatenation") {
        REQUIRE(dut == bitstring::bit_array("0b11101001"));
      }
    }
  }
}
