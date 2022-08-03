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
    WHEN("appending some bits") {
      dut.append("0b101101001");
      THEN("data must be appended correctly") {
        REQUIRE(dut == bitstring::bit_array("0b1011010101101001"));
      }
    }
    WHEN("appending empty bitvector") {
      dut.append(bitstring::bit_array(0U, 0));
      THEN("bitvector must be unmodified") {
        REQUIRE(dut == bitstring::bit_array("0b1011010"));
      }
    }
  }
}

SCENARIO("prepending to bit array") {
  GIVEN("a bit array") {
    auto dut = bitstring::bit_array("0b110101001");
    WHEN("prepending once") {
      dut.prepend("0b100110");
      THEN("binary conversion must succeed") {
        REQUIRE(dut.bin() == "100110110101001");
      }
      THEN("data must be prepended correctly") {
        REQUIRE(dut == bitstring::bit_array("0b100110'110101001"));
      }
    }
  }
}

SCENARIO("reserving space") {
  GIVEN("a small bit array") {
    auto dut = bitstring::bit_array("0b110101110100011111000111110011111001");
    const auto ref = dut;
    CHECK(dut.data().size() > 1); // required for 'less than necessary' test
    WHEN("reserving so much space that additional storage units are needed") {
      dut.reserve(3 * 8 * sizeof(decltype(dut)::storage_type));
      THEN("internal buffer must be reserved accordingly") {
        REQUIRE(dut.data().capacity() == 3);
      }
      THEN("bit array value must be unchanged") { REQUIRE(dut == ref); }
    }
    WHEN(
        "reserving so little space that no additional storage unit is needed") {
      dut.reserve(dut.size() + 1);
      THEN("available space must be unchagned") {
        REQUIRE(dut.data().size() == ref.data().size());
      }
      THEN("bit array value must be unchanged") { REQUIRE(dut == ref); }
    }
    WHEN("reserving less space than already available") {
      dut.reserve(1);
      THEN("available space must be unchagned") {
        REQUIRE(dut.data().size() == ref.data().size());
      }
      THEN("bit array value must be unchanged") { REQUIRE(dut == ref); }
    }
  }
}

SCENARIO("reserving space at the front") {
  GIVEN("an aligned bit array") {
    auto dut = bitstring::bit_array("0b1001");
    const auto ref = dut;
    CHECK(dut.data().size() == 1);
    WHEN("reserving a bit in front") {
      dut.reserve_front(1);
      THEN("an empty unit must be added in front") {
        REQUIRE(dut.data()[0] == 0);
      }
      THEN("bit array value must be unchanged") { REQUIRE(dut == ref); }
    }
    WHEN("reserving multiple units in front") {
      dut.reserve_front(3 * 64);
      THEN("empty units must be added in front") {
        REQUIRE(dut.data()[0] == 0);
        REQUIRE(dut.data()[1] == 0);
      }
      THEN("bit array value must be unchanged") { REQUIRE(dut == ref); }
    }
  }
  GIVEN("an unaligned bit array") {
    auto dut = bitstring::bit_array("0b1001");
    dut.prepend("0b1");
    const auto ref = dut;
    WHEN("reserving a bit in front") {
      dut.reserve_front(1);
      THEN("no additional space must be added") { REQUIRE(dut.data()[0] != 0); }
      THEN("bit array value must be unchanged") { REQUIRE(dut == ref); }
    }
  }
}

SCENARIO("extracting bits") {
  GIVEN("aligned bit array") {
    auto dut = bitstring::bit_array("0b10010011101000111101");
    WHEN("getting front bit") {
      auto extracted = dut.front(1);
      THEN("result must be single bit array") {
        REQUIRE(extracted == bitstring::bit_array("0b1"));
      }
    }
    WHEN("getting front 10 bits") {
      auto extracted = dut.front(10);
      THEN("result must be first 10 bits of DUT") {
        REQUIRE(extracted == bitstring::bit_array("0b1001001110"));
      }
    }
  }
  GIVEN("misalined bit array") {
    auto dut = bitstring::bit_array("0b101100101001110");
    dut.prepend(bitstring::bit_array("0b010001"));
    WHEN("getting front bit") {
      auto extracted = dut.front(1);
      THEN("result must be single bit array") {
        REQUIRE(extracted == bitstring::bit_array("0b0"));
      }
    }
    WHEN("getting front 10 bits") {
      auto extracted = dut.front(10);
      THEN("result must be first 10 bits of DUT") {
        REQUIRE(extracted == bitstring::bit_array("0b0100011011"));
      }
    }
  }
}
