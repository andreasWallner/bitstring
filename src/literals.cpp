#include "bitstring/literals.hpp"

namespace bitstring::literals {

bit_array operator"" _ba(const char *str, std::size_t size) {
  return bitstring::bit_array(std::string_view(str, size));
}

} // namespace bitstring::literals
