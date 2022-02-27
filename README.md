# C++ bitstring

WIP C++17 library to handle sequences of bits (similar to
std::vector<bool>). It allows for creation, concatenation,
splitting, iteration etc. of bit-sequences.

The library is mainly used by me to write unittests for
embedded system code, therefore performance is not optimized
in many places.

A big inspiration for the interface was the bitstring python
library I greatly enjoy (see https://bitstring.readthedocs.io/en/latest/index.html).
While python bitstring was a big inspiration, C++ bitstring
is not a carbon copy and behavior differs in places especially
to comply with C++ conventions.

## Using the library

The easiest way is to integrate bitstring into your build
via cmake CPM:

    CPMAddPackage("gh:andreasWallner/bitstring@<version>")

## Example

    #include "bitstring.hpp"

    void foo() {
      using namespace bitstring;

      // generate 10 bit sequence by parsing string
      auto const header = "0b1101000101"_ba;
      // generate 14 bit sequence from integer
      auto const data = bit_array(0xabcdU, 14);
      // generate 16 bit sequence from integer, but add MSB-first
      auto const footer = bit_array(uint16_t{0x1234U}, bitorder::msb_first);

      // concatenate all 3 (yes operator+ is currently missing)
      auto packet = bit_array(header).append(data).append(footer);

      // print concatenated sequence
      std::cout << packet.bin();
    }

## Development notes

To easily run clang-tidy during the build set `BITSTRING_CLANG_TIDY` to you clang-tidy.
You can also set it to e.g. `clang-tidy-10;-fix` to automatically apply fixes during the build.
