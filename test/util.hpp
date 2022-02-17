#ifndef header_bitstring_test_util_hpp
#define header_bitstring_test_util_hpp

#include "bitstring/bit_array.hpp"
#include <iostream>

std::ostream &operator<<(std::ostream &os, const bitstring::bit_array &bs);

#endif