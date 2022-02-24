#include "bitstring/bit_array.hpp"
#include "bitstring/exceptions.hpp"

#include <algorithm>

namespace bitstring {

bit_array::bit_array() : bitcnt_(0) {}

bit_array::bit_array(std::string_view s) : bitcnt_(0) {
  if (s.substr(0, 2) == "0b") {
    bits_.reserve(storage_units(s.size() - 2));
    storage_type e = 0;
    unsigned bits = 0;
    for (auto c : s.substr(2)) {
      if (c == '\'' || c == '_')
        continue;

      if (c != '0' && c != '1')
        throw parse_error("invalid character in bitstring");

      e |= (c - '0') << bits++;
      if (bits == sizeof(storage_type) * 8) {
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
      bitcnt_(8 * vec.size()) {
  size_t bits_idx = 0;
  size_t vec_idx = 0;
  for (; bits_idx < vec.size() / sizeof(storage_type); bits_idx++) {
    storage_type e = 0;
    for (size_t j = 0; j < sizeof(storage_type); j++, vec_idx++) {
      e |= static_cast<storage_type>(vec[vec_idx]) << (j * 8);
    }
    bits_[bits_idx] = e;
  }
  {
    storage_type e = 0;
    for (size_t j = 0; vec_idx < vec.size(); j++, vec_idx++) {
      e |= static_cast<storage_type>(vec[vec_idx]) << (j * 8);
    }
    bits_[bits_idx] = e;
  }
}

bool bit_array::operator==(const bit_array &other) const {
  if (this->size() != other.size())
    return false;

  if (!std::equal(begin(this->bits_), end(this->bits_) - 1, begin(other.bits_)))
    return false;

  auto const [last_offset, last_index] = split_index(this->size() - 1);
  auto const last_bit = (size_t{1} << last_offset);
  auto const mask = (last_bit - 1) | last_bit;
  return (this->bits_[last_index] & mask) == (other.bits_[last_index] & mask);
}

bool bit_array::operator!=(const bit_array &other) const {
  return !(*this == other);
}

uint8_t bit_array::operator[](const bitcnt_t idx) const {
  auto [offset, word] = split_index(idx);
  return static_cast<uint8_t>((bits_[word] >> offset) & 1);
}

std::size_t bit_array::size() const { return bitcnt_; }

void bit_array::reserve(bitcnt_t cnt) {
  bits_.reserve(storage_units(cnt));
}

std::string bit_array::bin() const {
  auto ret = std::string(bitcnt_, '0');
  for (size_t i = 0; i < bitcnt_; i++) {
    auto [bitpos, idx] = split_index(i);
    if (bits_[idx] & (storage_type{1} << bitpos)) {
      ret[i] = '1';
    }
  }
  return ret;
}

bit_array &bit_array::append(const bit_array &b) {
  const auto needed_size = storage_units(bitcnt_ + b.bitcnt_);
  bits_.resize(needed_size);

  for (size_t ib = 0; ib < b.bitcnt_; ib++) {
    auto [this_offset, this_idx] = split_index(bitcnt_ + ib);
    bits_[this_idx] |= b[ib] << this_offset;
  }

  // modify bitcnt_ only now such that self appending works
  bitcnt_ += b.bitcnt_;
  return *this;
}

bit_array &bit_array::append(std::string_view s) {
  return this->append(bit_array(s)); // to the trivial route for now
}

const std::vector<bit_array::storage_type> &bit_array::data() const {
  return bits_;
}

bit_array operator*(size_t cnt, const bit_array &ba) {
  bit_array result;
  while (cnt--)
    result.append(ba);
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
