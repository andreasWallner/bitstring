#include "bitstring/bit_array.hpp"
#include "bitstring/exceptions.hpp"

#include <algorithm>
#include <iostream>

namespace bitstring {

constexpr int bits_per_byte = 8;

bit_array::bit_array() : bitcnt_(0), offset_(0) {}

bit_array::bit_array(std::string_view s) : bitcnt_(0), offset_(0) {
  if (s.substr(0, 2) == "0b") {
    bits_.reserve(storage_units(s.size() - 2));
    storage_type e = 0;
    unsigned int bits = 0;
    for (auto c : s.substr(2)) {
      if (c == '\'' || c == '_') {
        continue;
      }

      if (c != '0' && c != '1') {
        throw parse_error("invalid character in bitstring");
      }

      e |= static_cast<storage_type>(c - '0') << bits++;
      if (bits == sizeof(storage_type) * bits_per_byte) {
        bitcnt_ += bits;
        bits_.push_back(e);
        bits = 0;
        e = 0;
      }
    }
    if (bits != 0) {
      bits_.push_back(e);
      bitcnt_ += bits;
    }
  } else {
    throw parse_error("invalid literal prefix");
  }
}

bit_array::bit_array(std::vector<uint8_t> vec)
    : bits_((vec.size() + sizeof(storage_type) - 1) / sizeof(storage_type)),
      bitcnt_(bits_per_byte * vec.size()), offset_(0) {
  size_t bits_idx = 0;
  size_t vec_idx = 0;
  for (; bits_idx < vec.size() / sizeof(storage_type); bits_idx++) {
    storage_type e = 0;
    for (size_t j = 0; j < sizeof(storage_type); j++, vec_idx++) {
      e |= static_cast<storage_type>(vec[vec_idx]) << (j * bits_per_byte);
    }
    bits_[bits_idx] = e;
  }
  {
    storage_type e = 0;
    for (size_t j = 0; vec_idx < vec.size(); j++, vec_idx++) {
      e |= static_cast<storage_type>(vec[vec_idx]) << (j * bits_per_byte);
    }
    bits_[bits_idx] = e;
  }
}

bool bit_array::operator==(const bit_array &other) const noexcept {
  if (this->size() != other.size()) {
    return false;
  }

  if (this->offset_ == 0 && other.offset_ == 0) {
    return compare_fast(other);
  }
  return compare_slow(other);
}

bool bit_array::compare_fast(const bit_array &other) const noexcept {
  if (this->bits_.size() > 1 &&
      !std::equal(begin(this->bits_), end(this->bits_) - 1,
                  begin(other.bits_))) {
    return false;
  }

  auto const last = shifted_idx(this->size() - 1);
  auto const last_bit = (size_t{1} << last.bit_offset());
  auto const mask = (last_bit - 1) | last_bit;
  return (this->bits_[last.unit()] & mask) == (other.bits_[last.unit()] & mask);
}

bool bit_array::compare_slow(const bit_array &other) const noexcept {
  for (bitcnt_t i = 0; i < size(); i++) {
    if ((*this)[i] != other[i]) {
      return false;
    }
  }
  return true;
}

bool bit_array::operator!=(const bit_array &other) const noexcept {
  return !(*this == other);
}

uint8_t bit_array::operator[](const bitcnt_t idx) const {
  auto split_idx = shifted_idx(idx);
  return static_cast<uint8_t>(
      (bits_[split_idx.unit()] >> split_idx.bit_offset()) & 1);
}

std::size_t bit_array::size() const { return bitcnt_; }

bool bit_array::empty() const { return bitcnt_ == 0; }

bit_array &bit_array::reserve(bitcnt_t cnt) {
  bits_.reserve(storage_units(cnt));
  return *this;
}

bit_array &bit_array::reserve_front(bitcnt_t cnt) {
  using diff_t = decltype(bits_)::difference_type;
  if (cnt < offset_) {
    return *this;
  }
  auto units_needed = storage_units(cnt);
  std::vector<storage_type> extended(units_needed + bits_.capacity(), 0);
  std::copy(begin(bits_), end(bits_),
            begin(extended) + static_cast<diff_t>(units_needed));
  swap(bits_, extended);
  offset_ = sizeof(storage_type) * 8 * units_needed;
  return *this;
}

std::string bit_array::bin() const {
  auto ret = std::string(bitcnt_, '0');
  for (size_t i = 0; i < bitcnt_; i++) {
    auto idx = shifted_idx(i);
    const auto unit = bits_[idx.unit()];
    const auto mask = idx.bit_mask();
    const auto bit = unit & mask;
    if (bit != 0U) {
      ret[i] = '1';
    }
  }
  return ret;
}

bit_array &bit_array::append(bool bit) {
  const auto needed_size = storage_units(bitcnt_ + 1);
  bits_.resize(needed_size);

  auto idx = shifted_idx(bitcnt_);
  bits_[idx.unit()] |= static_cast<storage_type>(bit) << idx.bit_offset();
  bitcnt_++;
  return *this;
}

bit_array &bit_array::append(const bit_array &b) {
  const auto needed_size = storage_units(bitcnt_ + b.bitcnt_);
  bits_.resize(needed_size);

  for (size_t ib = 0; ib < b.bitcnt_; ib++) {
    auto this_idx = shifted_idx(bitcnt_ + ib);
    bits_[this_idx.unit()] |= static_cast<storage_type>(b[ib])
                              << this_idx.bit_offset();
  }

  // modify bitcnt_ only now such that self appending works
  bitcnt_ += b.bitcnt_;
  return *this;
}

bit_array &bit_array::append(std::string_view s) {
  return this->append(bit_array(s)); // do the trivial route for now
}

bool bit_array::starts_with(const bit_array &other) const noexcept {
  if (other.size() > size()) {
    return false;
  }
  for (bitcnt_t b = 0; b < other.size(); b++) {
    if ((*this)[b] != other[b]) {
      return false;
    }
  }
  return true;
}

bit_array &bit_array::prepend(const bit_array &b) {
  const auto needed_units = storage_units(b.bitcnt_);
  bits_.insert(cbegin(bits_), needed_units, storage_type{0});
  offset_ += needed_units * sizeof(storage_type) * bits_per_byte - b.bitcnt_;

  for (bitcnt_t i = 0; i < b.bitcnt_; i++) {
    auto idx = shifted_idx(i);
    bits_[idx.unit()] |= static_cast<storage_type>(b[i]) << idx.bit_offset();
  }

  bitcnt_ += b.bitcnt_;
  return *this;
}

bit_array &bit_array::prepend(std::string_view s) {
  return this->prepend(bit_array(s));
}

bit_array bit_array::front(bitcnt_t bits) {
  bit_array other;

  using diff_t = decltype(bits_)::iterator::difference_type;
  const auto units =
      static_cast<diff_t>(storage_units(bits) + (offset_ == 0 ? 0 : 1));
  const auto source_offset_units =
      static_cast<diff_t>(storage_units(offset_) - (offset_ == 0 ? 0 : 1));

  other.bits_.reserve(static_cast<size_t>(units));
  std::copy(begin(bits_) + source_offset_units,
            begin(bits_) + source_offset_units + units, begin(other.bits_));
  other.offset_ = offset_;
  other.bitcnt_ = bits;

  return other;
}

const std::vector<bit_array::storage_type> &bit_array::data() const {
  return bits_;
}

bit_array operator*(size_t cnt, const bit_array &ba) {
  bit_array result;
  result.reserve(cnt * ba.size());
  while (cnt-- > 0) {
    result.append(ba);
  }
  return result;
}

bit_array operator*(const bit_array &ba, size_t cnt) { return cnt * ba; }

bit_array operator+(const bit_array &left, const bit_array &right) {
  bit_array result;
  result.append(left);
  result.append(right);
  return result;
}

} // namespace bitstring
