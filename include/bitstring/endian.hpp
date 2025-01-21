#ifndef header_bitstring_endian_hpp
#define header_bitstring_endian_hpp

#include <cstdint>

namespace bitstring {
enum class bitorder { lsb_first, msb_first };

namespace detail {
template <typename T> struct half_type {};
template <> struct half_type<uint64_t> {
  using type = uint32_t;
};
template <> struct half_type<uint32_t> {
  using type = uint16_t;
};
template <> struct half_type<uint16_t> {
  using type = uint8_t;
};
template <typename T> using half_type_t = typename half_type<T>::type;

template <typename T> constexpr T bitflipped(T v) noexcept;

template <> constexpr uint8_t bitflipped<uint8_t>(uint8_t v) noexcept {
  v = static_cast<uint8_t>(((v & 0b10101010) >> 1) |
                           ((v & 0b01010101) << 1)); // 12345678 => 21436587
  v = static_cast<uint8_t>(((v & 0b11001100) >> 2) |
                           ((v & 0b00110011) << 2)); //          => 43218765
  v = static_cast<uint8_t>(((v & 0b11110000) >> 4) |
                           ((v & 0b00001111) << 4)); //          => 87654321
  return v;
}

template <typename T> constexpr T bitflipped(T v) noexcept {
  constexpr auto shift_width = 8 * sizeof(half_type_t<T>);
  const T flipped_upper = bitflipped<half_type_t<T>>(v >> shift_width);
  const T flipped_lower = bitflipped<half_type_t<T>>(v);
  return (flipped_lower << shift_width) | flipped_upper;
}
} // namespace detail
} // namespace bitstring

#endif
