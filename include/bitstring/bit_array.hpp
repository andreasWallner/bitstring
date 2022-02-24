#ifndef header_bitstring_bit_array_hpp
#define header_bitstring_bit_array_hpp

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "bitstring/endian.hpp"

namespace bitstring {

class bit_array {
public:
  using storage_type = std::uint32_t;
  using bitoffset_t = std::size_t;

private:
  std::vector<storage_type> bits_;
  std::size_t bitcnt_;

public:
  bit_array();
  explicit bit_array(std::string_view);
  explicit bit_array(std::vector<uint8_t>);
  template <typename T,
            std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> &&
                                 !std::is_same_v<T, bool>,
                             int> = 0>
  bit_array(T, bitorder bio = bitorder::lsb_first);
  template <typename T,
            std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> &&
                                 !std::is_same_v<T, bool>,
                             int> = 0>
  bit_array(T, size_t len, bitorder bio = bitorder::lsb_first);
  // TODO int constructor with template parameters?
  // TODO more constexpr?

  bool operator==(const bit_array &other) const;
  bool operator!=(const bit_array &other) const;
  uint8_t operator[](bitoffset_t) const;
  // TODO: bitproxy operator[](bitoffset_t);

  size_t size() const;

  std::string bin() const;
  std::string hex() const;
  template <typename T> T as_int(bitorder /*, byteorder*/) const;

  bit_array &append(const bit_array &b);
  bit_array &append(std::string_view);

  // void prepend(const bit_array &b);
  // front
  // back
  // substr
  // insert
  // erase
  // overwrite
  // iterate bits
  // iterate slices
  // iterate split
  // find

  const std::vector<storage_type> &data() const;

public:
  template <typename T>
  static constexpr size_t storage_units(size_t cnt) noexcept {
    size_t bytes = sizeof(T) * cnt;
    return (bytes + sizeof(storage_type) - 1) / sizeof(storage_type);
  }

  static constexpr size_t storage_units(size_t bits) noexcept {
    return storage_units<uint8_t>((bits + 7) / 8);
  }

  static constexpr unsigned int log2ceil(std::uintmax_t n) noexcept {
    unsigned ret = 0;
    while (n > 0) {
      ret++;
      n /= 2;
    }
    return ret;
  }

  static constexpr std::pair<size_t, size_t>
  split_index(size_t bitindex) noexcept {
    constexpr size_t bitpos_mask =
        (size_t{1} << log2ceil(sizeof(storage_type) * 8 - 1)) - 1;
    constexpr size_t storage_idx_offset =
        log2ceil(sizeof(storage_type) * 8 - 1);
    return std::make_pair(bitindex & bitpos_mask,
                          bitindex >> storage_idx_offset);
  }
};

template <typename T,
          std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> &&
                               !std::is_same_v<T, bool>,
                           int>>
bit_array::bit_array(T v, bitorder bio) : bit_array(v, sizeof(v) * 8, bio) {}

template <typename T,
          std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> &&
                               !std::is_same_v<T, bool>,
                           int>>
bit_array::bit_array(T v, size_t bits, bitorder bio /* = lsb_first */)
    : bits_(storage_units(bits), storage_type{0}), bitcnt_(bits) {
  if (bits > (8 * sizeof(T)))
    return; // not implemented yet - more to think
  if (bio == bitorder::msb_first) {
    v = detail::bitflipped(v);
    v >>= 8 * sizeof(T) - bits;
  }
  for (size_t i = 0; i < storage_units<T>(1); i++) {
    // TODO: think about UB in the shift
    auto unit = static_cast<storage_type>(v >> (sizeof(storage_type) * i * 8));
    bits_[i] = unit;
  }
}

bit_array operator*(size_t cnt, const bit_array &ba);
bit_array operator*(const bit_array &ba, size_t cnt);
bit_array operator+(const bit_array &left, const bit_array &right);

} // namespace bitstring

#endif
