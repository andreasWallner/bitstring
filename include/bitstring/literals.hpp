#ifndef header_bitstring_literals_hpp
#define header_bitstring_literals_hpp

#include "bitstring/bit_array.hpp"

namespace bitstring::literals {

bitstring::bit_array operator"" _ba(const char *, std::size_t);

}

#endif
