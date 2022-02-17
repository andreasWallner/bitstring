#include "util.hpp"

std::ostream &operator<<(std::ostream &os, const bitstring::bit_array &bs) {
  os << "0b" << bs.bin();
  return os;
}