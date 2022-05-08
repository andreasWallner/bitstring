#include "bitstring/bit_array.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("format binary") {
  GIVEN("clean bit_array") {
    WHEN("formatting") {
      THEN("full byte must be correct") {
        REQUIRE(bitstring::bit_array(uint8_t{0x45U}).bin() == "10100010");
      }
      THEN("cut off byte must be correct") {
        REQUIRE(bitstring::bit_array(0x45U, 7).bin() == "1010001");
      }
      THEN("single bit must be correct") {
        REQUIRE(bitstring::bit_array(0x1U, 1).bin() == "1");
      }
    }
  }

  GIVEN("multi-unit bit array") {
    std::vector<uint8_t> vec{0b10010011, 0b11000111, 0b10101011,
                             0b00110010, 0b00001000, 0b10000100,
                             0b11111110, 0b00000001, 0b10101111};
    auto dut = bitstring::bit_array(vec);
    WHEN("formatting") {
      THEN("bits must be correct") {
        REQUIRE(dut.bin() == "1100100111100011110101010100110000010000001000010"
                             "11111111000000011110101");
      }
    }
  }
  GIVEN("bit array with possible garbage in storage") {
    WHEN("formatting") {
      THEN("garbage must not be printed") {
        REQUIRE(bitstring::bit_array(0xFFFF58421U, 17).bin() ==
                "10000100001000011");
      }
    }
  }
}