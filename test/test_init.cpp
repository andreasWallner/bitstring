#include "bitstring/bit_array.hpp"
#include "bitstring/exceptions.hpp"
#include "bitstring/literals.hpp"
#include "util.hpp"

#include <catch2/catch_test_macros.hpp>

SCENARIO("default initialization") {
  GIVEN("no preconditions") {
    WHEN("using default constructor") {
      auto dut = bitstring::bit_array();
      THEN("must be shown as empty") { REQUIRE(dut.empty()); }
      THEN("size must be 0") { REQUIRE(dut.size() == 0); } // NOLINT
      THEN("string conversion must be empty") { REQUIRE(dut.bin().empty()); }
    }
  }
}

SCENARIO("init from binary data") {
  GIVEN("no precondition") {
    WHEN("constructing with short vector (< storage type)") {
      static_assert(sizeof(bitstring::bit_array::storage_type) > 2);
      std::vector<uint8_t> test_vector{0xaa, 0xbb};

      auto dut = bitstring::bit_array(test_vector);
      THEN("bit_array must have correct size and content") {
        REQUIRE(dut.size() == 16);
        REQUIRE(dut.data().size() == 1);
        REQUIRE(dut.data()[0] == 0xbbaa);
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
    WHEN("constructing with vector (= storage_type size)") {
      static_assert(sizeof(bitstring::bit_array::storage_type) == 4);
      std::vector<uint8_t> test_vector{0x11, 0x22, 0x33, 0x44};

      auto dut = bitstring::bit_array{test_vector};
      THEN("bit_array must have correct size and content") {
        REQUIRE(dut.size() == 32);
        REQUIRE(dut.data().size() == 1);
        REQUIRE(dut.data()[0] == 0x44332211);
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }

    WHEN("constructing with long vector (> storage_type size)") {
      static_assert(sizeof(bitstring::bit_array::storage_type) < 9);
      std::vector<uint8_t> test_vector{0x99, 0x88, 0x77, 0x66, 0x55,
                                       0x44, 0x33, 0x22, 0x11};

      auto dut = bitstring::bit_array(test_vector);
      THEN("bit_array_must have correct size and content") {
        REQUIRE(dut.size() == 8 * 9);
        REQUIRE(dut.data().size() == 3);
        REQUIRE(dut.data()[0] == 0x66778899);
        REQUIRE(dut.data()[1] == 0x22334455);
        REQUIRE(dut.data()[2] == 0x11);
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
  }
}

SCENARIO("init from integer") {
  GIVEN("no precondition") {
    WHEN("initializing from uint8_t") {
      auto dut = bitstring::bit_array(uint8_t{0b01000101});
      THEN("size must be 8") { REQUIRE(dut.size() == 8); }
      THEN("internal storage must be minimal") {
        REQUIRE(dut.data().size() == 1);
      }
      THEN("data must be set") { REQUIRE(dut.data()[0] == 0b01000101); }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
    WHEN("initializing from uint8_t, big endian") {
      auto dut =
          bitstring::bit_array(uint8_t{0x45}, bitstring::bitorder::msb_first);
      THEN("size must be 8") { REQUIRE(dut.size() == 8); }
      THEN("data must be set") { REQUIRE(dut.data()[0] == 0b10100010); }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
    WHEN("initializing from part of uint8_t") {
      auto dut = bitstring::bit_array(uint8_t{0x54}, 5);
      THEN("size must be match passed len") { REQUIRE(dut.size() == 5); }
      THEN("internal storage must be minimal") {
        REQUIRE(dut.data().size() == 1);
      }
      THEN("significant bits must be set") {
        REQUIRE((dut.data()[0] & 0b11111) == 0x14);
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
    WHEN("initializing from part of uint8_t, big endian") {
      auto dut = bitstring::bit_array(uint8_t{0b01110100}, 5,
                                      bitstring::bitorder::msb_first);
      THEN("size must be match passed len") { REQUIRE(dut.size() == 5); }
      THEN("internal storage must be minimal") {
        REQUIRE(dut.data().size() == 1);
      }
      THEN("significant bits must be set") {
        REQUIRE((dut.data()[0] & 0b11111) == 0b00101);
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
    WHEN("initializing from 0 bits of uint8_t") {
      auto dut = bitstring::bit_array(uint8_t{0xff}, 0);
      THEN("length must be 0") { REQUIRE(dut.size() == 0); } // NOLINT
      THEN("must report as empty") { REQUIRE(dut.empty()); }
      THEN("must print as empty string") { REQUIRE(dut.bin() == ""); } // NOLINT
    }
    WHEN("initializing from 0 bits of uint8_t, big endian") {
      auto dut = bitstring::bit_array(uint8_t{0xff}, 0,
                                      bitstring::bitorder::msb_first);
      THEN("length must be 0") { REQUIRE(dut.size() == 0); } // NOLINT
      THEN("must report as empty") { REQUIRE(dut.empty()); }
      THEN("must print as empty string") { REQUIRE(dut.bin() == ""); } // NOLINT
    }
    /*WHEN("extending from uint8_t, bit endian", "[!mayfail]") {
      auto dut = bitstring::bit_array(uint8_t{0x74}, 20,
    bitstring::bitorder::big); THEN("size must be match passed len") {
    REQUIRE(dut.size() == 20); } THEN("internal storage must be minimal") {
        REQUIRE(dut.data().size() == 1);
      }
      THEN("significant bits must be set") {
        REQUIRE((dut.data()[0] & 0b1111'11111111'11111111) ==
    0b0000'00000000'00101110);
      }
    }*/
    /*WHEN("extending from uint8_t") {
      auto dut = bitstring::bit_array(uint8_t{0x45}, 40);
      THEN("size must match passed len") { REQUIRE(dut.size() == 40); }
      THEN("internal storage must be minimal") {
        REQUIRE(dut.data().size() == 2);
      }
      THEN("data must be set") { REQUIRE(dut.data()[0] == 0x45); }
      THEN("data must be extended with 0") {
        REQUIRE((dut.data()[1] & 0xff) == 0);
      }
    }*/

    WHEN("initializing from uint32_t") {
      auto dut = bitstring::bit_array(uint32_t{0x11223344});
      THEN("size must be 32") { REQUIRE(dut.size() == 32); }
      THEN("internal storage must be minimal") {
        REQUIRE(dut.data().size() == 1);
      }
      THEN("data must be set") { REQUIRE(dut.data()[0] == 0x11223344); }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
    WHEN("initializing from uint32_t, big endian") {
      auto dut = bitstring::bit_array(uint32_t{0x11223344},
                                      bitstring::bitorder::msb_first);
      THEN("size must be 32") { REQUIRE(dut.size() == 32); }
      THEN("internal storage must be minimal") {
        REQUIRE(dut.data().size() == 1);
      }
      THEN("data must be set") {
        REQUIRE(dut.data()[0] == 0b00100010'11001100'01000100'10001000);
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }

    WHEN("initializing from uint64_t") {
      auto dut = bitstring::bit_array(UINT64_C(0x11223344aabbccdd));
      THEN("size must be 32") { REQUIRE(dut.size() == 64); }
      THEN("internal storage must be minimal") {
        REQUIRE(dut.data().size() == 2);
      }
      THEN("data must be set") {
        REQUIRE(dut.data()[0] == 0xaabbccdd);
        REQUIRE(dut.data()[1] == 0x11223344);
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
    WHEN("initializing from part of uint64_t") {
      auto dut = bitstring::bit_array(UINT64_C(0x11223344aabbccdd), 60);
      THEN("size must be match passed len") { REQUIRE(dut.size() == 60); }
      THEN("internal storage must be minimal") {
        REQUIRE(dut.data().size() == 2);
      }
      THEN("significant data must be set") {
        REQUIRE(dut.data()[0] == 0xaabbccdd);
        REQUIRE((dut.data()[1] & 0x0fffffff) == 0x01223344);
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
    /*WHEN("extending from uint64_t") {
      auto dut = bitstring::bit_array(UINT64_C(0x11223344aabbccdd), 70);
      THEN("size must be match passed len") { REQUIRE(dut.size() == 70); }
      THEN("internal storage must be minimal") {
        REQUIRE(dut.data().size() == 3);
      }
      THEN("significant data must be set") {
        REQUIRE(dut.data()[0] == 0xaabbccdd);
        REQUIRE(dut.data()[1] == 0x11223344);
      }
      THEN("data must be extended with 0") {
        REQUIRE((dut.data()[2] & 0x3f) == 0);
      }
    }*/
  }
}

SCENARIO("init from string") {
  GIVEN("short binary string") {
    const std::string s{"0b101101"};
    WHEN("constructing bit_array") {
      auto dut = bitstring::bit_array(s);
      THEN("must be parsed correctly") {
        REQUIRE(dut == bitstring::bit_array(0b101101U, 6));
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
    WHEN("using UDL to construct bit_array") {
      using namespace bitstring::literals;
      auto dut = "0b101101"_ba;
      THEN("must be parsed like normal init") {
        REQUIRE(dut == bitstring::bit_array(0b101101U, 6));
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
  }

  GIVEN("string with 1 whole storage unit") {
    static_assert(sizeof(bitstring::bit_array::storage_type) == 4);
    const std::string s{"0b"
                        "10010011"
                        "11000111"
                        "10101011"
                        "00110010"};
    WHEN("constructing bit_array") {
      auto dut = bitstring::bit_array(s);
      THEN("must be parsed correctly") {
        REQUIRE(dut == bitstring::bit_array(
                           0b01001100'11010101'11100011'11001001U, 32));
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
  }

  GIVEN("string with > 2 whole storage units") {
    static_assert(sizeof(bitstring::bit_array::storage_type) == 4);
    const std::string s{"0b"
                        "11001001"
                        "11100011"
                        "11010101"
                        "01001100"
                        "00010000"
                        "00100001"
                        "01111111"
                        "10000000"
                        "11110101"};

    WHEN("constructing bit_array") {
      auto dut = bitstring::bit_array(s);
      THEN("must be parsed correctly") {
        std::vector<uint8_t> expected{0b10010011, 0b11000111, 0b10101011,
                                      0b00110010, 0b00001000, 0b10000100,
                                      0b11111110, 0b00000001, 0b10101111};
        auto b = bitstring::bit_array(expected);
        REQUIRE(dut == bitstring::bit_array(expected));
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
  }

  GIVEN("string with seperators") {
    const std::string s{"0b1'00101'101_011"};
    WHEN("constructing bit_array") {
      auto dut = bitstring::bit_array(s);
      THEN("must be parsed correctly") {
        REQUIRE(dut == bitstring::bit_array("0b100101101011"));
      }
      THEN("bit_array must not be empty") { REQUIRE_FALSE(dut.empty()); }
    }
  }

  GIVEN("string with invalid prefix") {
    const std::string s{"0e010100101010"};
    WHEN("constructing bit_array from it") {
      THEN("parse_error must be thrown") {
        REQUIRE_THROWS_AS(bitstring::bit_array(s), bitstring::parse_error);
      }
    }
  }

  GIVEN("string with invalid characters") {
    const std::string s{"0b11010121001"};
    WHEN("constructing bit_array from it") {
      THEN("parse_error must be thrown") {
        REQUIRE_THROWS_AS(bitstring::bit_array(s), bitstring::parse_error);
      }
    }
  }
}
