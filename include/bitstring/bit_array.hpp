#ifndef header_bitstring_bit_array_hpp
#define header_bitstring_bit_array_hpp

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "bitstring/endian.hpp"

namespace bitstring {

namespace detail {
static constexpr unsigned int log2ceil(std::uintmax_t n) noexcept {
  unsigned ret = 0;
  while (n > 0) {
    ret++;
    n /= 2;
  }
  return ret;
}

class bit_index {
  using storage_type_t = std::uint32_t;
  using bitcnt_t = std::size_t;
  using offset_t = std::size_t;
  static constexpr int offset_bits{2 + log2ceil(sizeof(storage_type_t))};
  static constexpr size_t offset_mask{
      (size_t{1} << log2ceil(sizeof(storage_type_t) * 8 - 1)) - 1};
  std::size_t index_;

public:
  constexpr bit_index(std::size_t index) : index_(index) {}

  constexpr bitcnt_t unit() const noexcept { return index_ >> offset_bits; }
  constexpr offset_t bit_offset() const noexcept {
    return index_ & offset_mask;
  }
  constexpr storage_type_t bit_mask() const noexcept {
    return storage_type_t{1} << bit_offset();
  }
};
} // namespace detail

class bit_array {
public:
  using storage_type = std::uint32_t;
  using bitcnt_t = std::size_t;

private:
  std::vector<storage_type> bits_;
  std::size_t bitcnt_;
  std::size_t offset_;

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
  uint8_t operator[](bitcnt_t) const;
  // TODO: bitproxy operator[](bitcnt_t);

  size_t size() const;
  bool empty() const;
  void reserve(bitcnt_t bitcnt);

  std::string bin() const;
  std::string hex() const;
  template <typename T> T as_int(bitorder /*, byteorder*/) const;

  bit_array &append(const bit_array &b);
  bit_array &append(std::string_view);
  bit_array &prepend(const bit_array &b);
  bit_array &prepend(std::string_view);
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

  bool starts_with(const bit_array &b) const noexcept;

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
    : bits_(storage_units(bits), storage_type{0}), bitcnt_(bits), offset_(0) {
  if (bits == 0)
    return;
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
