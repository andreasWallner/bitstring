#include "bitstring/endian.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("flipping bits") {
  using namespace bitstring::detail;
  REQUIRE(bitflipped<uint8_t>(0b11001011) == 0b11010011);
  REQUIRE(bitflipped<uint8_t>(0b11101001) == 0b10010111);

  REQUIRE(bitflipped<uint32_t>(0b01100010'01101110'10110111'01010001) ==
          0b10001010'11101101'01110110'01000110);
  REQUIRE(bitflipped<uint32_t>(0b11100011'11001000'10001100'00111001) ==
          0b10011100'00110001'00010011'11000111);
}
