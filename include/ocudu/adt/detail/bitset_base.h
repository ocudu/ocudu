// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/ranges/iota.h"
#include "ocudu/adt/ranges/transform.h"
#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/support/math/bit_ops.h"
#include "ocudu/support/ocudu_assert.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace ocudu {

namespace bitset_detail {

/// Integer type of a single word of a bitset.
using word_type = uint64_t;

/// Number of bits held by a single word of a bitset, regardless of its template parameters.
inline constexpr size_t bits_per_word = 8U * sizeof(word_type);

/// Index of the word that holds bit position \c bitidx.
constexpr size_t get_word_idx(size_t bitidx) noexcept
{
  return bitidx / bits_per_word;
}

/// Number of words required to hold \c size bits.
OCUDU_FORCE_INLINE constexpr size_t nof_words(size_t size) noexcept
{
  return (size + bits_per_word - 1) / bits_per_word;
}

/// Asserts that \c pos is a valid bit index of a bitset of size \c size.
constexpr void assert_within_bounds(size_t pos, size_t size, bool strict) noexcept
{
  ocudu_assert(pos < size or (not strict and pos == size),
               "ERROR: index='{}' is out-of-bounds for bitset of size='{}'",
               pos,
               size);
}

/// Asserts that ['startpos', 'endpos') is a valid bit index range of a bitset of size \c size.
constexpr void assert_range_bounds(size_t startpos, size_t endpos, size_t size) noexcept
{
  ocudu_assert(startpos <= endpos and endpos <= size,
               "ERROR: range ['{}', '{}') out-of-bounds for bitsize of size='{}'",
               startpos,
               endpos,
               size);
}

/// Transform predicate that computes, for a word index, the mask of bits (within that word) that fall inside
/// [start, stop).
///
/// \tparam LowestInfoBitIsMSB Bit index order in memory. See \c bounded_bitset for more details.
template <bool LowestInfoBitIsMSB>
struct word_mask_functor {
  /// Value yielded by the range: the index of a word intersecting [start, stop) and its selected-bit mask.
  struct value_type {
    size_t    word_idx;
    word_type mask;
  };

  size_t start_word;
  size_t end_word;
  size_t startmod;
  size_t stopmod;
  size_t tail_unused;

  value_type operator()(size_t idx) const noexcept
  {
    word_type mask = ~static_cast<word_type>(0);
    if (idx == start_word and startmod != 0) {
      if constexpr (LowestInfoBitIsMSB) {
        mask &= mask_msb_zeros<word_type>(startmod);
      } else {
        mask &= mask_lsb_zeros<word_type>(startmod);
      }
    }
    if (idx == end_word - 1 and stopmod != 0) {
      if constexpr (LowestInfoBitIsMSB) {
        mask &= mask_lsb_zeros<word_type>(tail_unused);
      } else {
        mask &= mask_msb_zeros<word_type>(tail_unused);
      }
    }
    return {idx, mask};
  }
};

} // namespace bitset_detail

namespace detail {

/// \brief CRTP base class holding the word buffer of a bitset with capacity \c N and the operations shared by
/// \c bounded_bitset and \c fixed_size_bitset.
///
/// \c Derived must expose a public <tt> size() const noexcept </tt> returning the current number of bits.
///
/// \tparam Derived            Concrete bitset type.
/// \tparam N                  Capacity of the bitset in bits.
/// \tparam LowestInfoBitIsMSB Bit index order in memory. See \c bounded_bitset for more details.
template <typename Derived, size_t N, bool LowestInfoBitIsMSB>
class bitset_crtp
{
protected:
  using word_t = bitset_detail::word_type;

public:
  static constexpr bool bit_order() noexcept { return LowestInfoBitIsMSB; }

  /// Number of bits held by a single word of the bitset.
  static constexpr size_t bits_per_word = bitset_detail::bits_per_word;

  /// Capacity of the bitset in bits.
  static constexpr size_t max_size() noexcept { return N; }

  /// Returns true if the bitset size is 0.
  OCUDU_FORCE_INLINE constexpr bool empty() const noexcept { return size_() == 0; }

  /// \brief Set bit with provided index to either true or false. Assertion is triggered if pos >= size().
  /// \param[in] pos Position in bitset.
  /// \param[in] val Value to set the bit.
  void set(size_t pos, bool val) noexcept
  {
    assert_within_bounds_(pos, true);
    set_(pos, val);
  }

  /// \brief Set bit with provided index to true. Assertion is triggered if pos >= size().
  /// \param[in] pos Position in bitset.
  void set(size_t pos) noexcept
  {
    assert_within_bounds_(pos, true);
    set_(pos);
  }

  /// \brief Set bit with provided index to false. Assertion is triggered if pos >= size().
  /// \param[in] pos Position in bitset.
  void reset(size_t pos) noexcept
  {
    assert_within_bounds_(pos, true);
    reset_(pos);
  }

  /// Reset all bits in the bitset to false. The size of the bitset is maintained.
  void reset() noexcept
  {
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      buffer[i] = static_cast<word_t>(0);
    }
  }

  /// Set all bits in the bitset to true/false. The size of the bitset is maintained.
  void fill(bool val = true) noexcept
  {
    if (not val) {
      reset();
      return;
    }
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      buffer[i] = static_cast<word_t>(-1);
    }
    sanitize_();
  }

  /// \brief Fills range of bits to either true or false.
  /// \param[in] startpos Starting bit index that will be set.
  /// \param[in] endpos End bit index (excluding) where the bits stop being set.
  /// \param[in] value Set bit range values to either true or false.
  /// \return Returns a reference to this object.
  Derived& fill(size_t startpos, size_t endpos, bool value = true) noexcept
  {
    for (const auto& [word_idx, mask] : make_word_mask_range(startpos, endpos)) {
      if (value) {
        buffer[word_idx] |= mask;
      } else {
        buffer[word_idx] &= ~mask;
      }
    }
    return derived_();
  }

  /// \brief Check if bit with provided index is set to true.
  /// \param[in] pos Position in bitset.
  /// \return Returns true if bit at position pos is set.
  [[nodiscard]] constexpr bool test(size_t pos) const noexcept
  {
    assert_within_bounds_(pos, true);
    return test_(pos);
  }

  /// Gets a pointer to the underlying array of raw words (i.e. \c bitset_detail::bits_per_word bits each).
  [[nodiscard]] constexpr const word_t* data() const noexcept { return buffer.data(); }

  /// \brief Toggle the value at position pos. Assertion is triggered if pos >= size().
  /// \param[in] pos Position in bitset.
  void flip(size_t pos) noexcept
  {
    assert_within_bounds_(pos, true);
    if (test(pos)) {
      reset_(pos);
    } else {
      set_(pos);
    }
  }

  /// \brief Toggle values of bits in bitset.
  /// \return Returns this object.
  Derived& flip() noexcept
  {
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      buffer[i] = ~buffer[i];
    }
    sanitize_();
    return derived_();
  }

  /// \brief Extracts \c nof_bits starting from \c startpos.
  ///
  /// \return An unsigned integer containing \c nof_bits of the set where starting with the most significant bit and
  /// finishing with the most significant bit.
  /// \remark An assertion is triggered if the bit range exceed the set size.
  template <typename Integer = unsigned>
  Integer extract(unsigned startpos, unsigned nof_bits) const noexcept
  {
    static_assert(std::is_unsigned_v<Integer>, "Extract only works for unsigned integers");
    ocudu_assert(nof_bits <= sizeof(Integer) * 8,
                 "The number of bits (i.e., {}) exceeds the destination bit-width (i.e., {}).",
                 nof_bits,
                 sizeof(Integer) * 8);
    ocudu_assert(startpos + nof_bits <= size_(),
                 "The start position (i.e., {}) plus the number of bits (i.e., {}) exceed the current size (i.e., {})",
                 startpos,
                 nof_bits,
                 size_());

    Integer val = 0;

    for (unsigned bit_index = 0; bit_index != nof_bits; ++bit_index) {
      if (test(startpos + bit_index)) {
        val |= Integer{1} << (nof_bits - 1 - bit_index);
      }
    }

    return val;
  }

  /// \brief Finds the lowest bit with value set to the value passed as argument.
  /// \param[in] value The bit value to find, either true (1) or false (0).
  /// \return Returns the lowest found bit index or -1 in case no bit was found with the provided value argument.
  int find_lowest(bool value = true) const noexcept { return find_lowest(0, size_(), value); }

  /// \brief Finds, within a range of bit indexes, the lowest bit with value set to the value passed as argument.
  /// \param[in] startpos Starting bit index for the search.
  /// \param[in] endpos End bit index for the search.
  /// \param[in] value The bit value to find, either true (1) or false (0).
  /// \return Returns the lowest found bit index or -1 in case no bit was found with the provided value argument.
  int find_lowest(size_t startpos, size_t endpos, bool value = true) const noexcept
  {
    for (const auto& [word_idx, mask] : make_word_mask_range(startpos, endpos)) {
      word_t w = value ? buffer[word_idx] : ~buffer[word_idx];
      w &= mask;
      if (w != 0) {
        // Found bit. Store its position.
        int pos = static_cast<int>(word_idx * bits_per_word);
        if constexpr (LowestInfoBitIsMSB) {
          pos += convert_bitpos_(find_first_msb_one(w));
        } else {
          pos += find_first_lsb_one(w);
        }
        return pos;
      }
    }
    return -1;
  }

  /// \brief Finds the highest bit with value set to the value passed as argument.
  /// \param[in] value The bit value to find, either true (1) or false (0).
  /// \return Returns the highest found bit index or -1 in case no bit was found with the provided value argument.
  int find_highest(bool value = true) const noexcept { return find_highest(0, size_(), value); }

  /// \brief Finds, within a range of bit indexes, the highest bit with value set to the value passed as argument.
  /// \param[in] startpos Starting bit index for the search.
  /// \param[in] endpos End bit index for the search.
  /// \param[in] value The bit value to find, either true (1) or false (0).
  /// \return Returns the highest found bit index or -1 in case no bit was found with the provided value argument.
  int find_highest(size_t startpos, size_t endpos, bool value = true) const noexcept
  {
    assert_range_bounds_(startpos, endpos);
    if (startpos == endpos) {
      return -1;
    }
    size_t startword = startpos / bits_per_word;
    size_t lastword  = (endpos - 1) / bits_per_word;

    for (size_t i = lastword; i != startword - 1; --i) {
      word_t w = buffer[i];
      if (not value) {
        w = ~w;
      }

      if (i == startword) {
        size_t removed_bits = startpos % bits_per_word;
        if constexpr (LowestInfoBitIsMSB) {
          w &= mask_msb_zeros<word_t>(removed_bits);
        } else {
          w &= mask_lsb_zeros<word_t>(removed_bits);
        }
      }
      if (i == lastword) {
        size_t kept_bits = ((endpos - 1) % bits_per_word) + 1;
        if constexpr (LowestInfoBitIsMSB) {
          w &= mask_msb_ones<word_t>(kept_bits);
        } else {
          w &= mask_lsb_ones<word_t>(kept_bits);
        }
      }
      if (w != 0) {
        if constexpr (LowestInfoBitIsMSB) {
          return static_cast<int>(i * bits_per_word + convert_bitpos_(find_first_lsb_one(w)));
        } else {
          return static_cast<int>(i * bits_per_word + find_first_msb_one(w));
        }
      }
    }
    return -1;
  }

  /// \brief Executes a function for all \c true (or all \c false) bits in the given bitset interval.
  ///
  /// \param[in] startpos Smallest bit index considered for the function execution (included).
  /// \param[in] endpos   Largest bit index considered for the function execution (excluded).
  /// \param[in] function Function to execute - the signature should be compatible with <tt>void ()(unsigned)</tt>.
  /// \param[in] value    Bit value that triggers the function execution.
  template <class T>
  void for_each(size_t startpos, size_t endpos, T&& function, bool value = true) const noexcept
  {
    static_assert(std::is_invocable_v<T&, size_t>, "The function must be invocable with a bit position.");
    static_assert(!LowestInfoBitIsMSB, "The for_each method is not yet available for reversed bitsets.");

    assert_range_bounds_(startpos, endpos);

    if (startpos == endpos) {
      return;
    }

    if ((value && all(startpos, endpos)) || (!value && none(startpos, endpos))) {
      for (size_t bitpos = startpos; bitpos != endpos; ++bitpos) {
        function(bitpos);
      }
      return;
    }

    size_t startword = startpos / bits_per_word;
    size_t lastword  = (endpos + bits_per_word - 1) / bits_per_word;
    for (size_t i = startword; i != lastword; ++i) {
      word_t w = buffer[i];
      if (not value) {
        w = ~w;
      }

      if (w == 0) {
        continue;
      }

      if (i == startword) {
        w &= mask_lsb_zeros<word_t>(startpos % bits_per_word);
      }

      if ((i == lastword - 1) && (endpos % bits_per_word != 0)) {
        w &= mask_lsb_ones<word_t>(endpos % bits_per_word);
      }

      // Process presets of 4 bits.
      unsigned bitpos = i * bits_per_word;
      for (; w != 0; w = w >> 4, bitpos += 4) {
        switch (w & 0xf) {
          case 0B0000:
            // All bits are false, skip.
            break;
          case 0B0001:
            function(bitpos + 0);
            break;
          case 0B0010:
            function(bitpos + 1);
            break;
          case 0B0011:
            function(bitpos + 0);
            function(bitpos + 1);
            break;
          case 0B0100:
            function(bitpos + 2);
            break;
          case 0B0101:
            function(bitpos + 0);
            function(bitpos + 2);
            break;
          case 0B0110:
            function(bitpos + 1);
            function(bitpos + 2);
            break;
          case 0B0111:
            function(bitpos + 0);
            function(bitpos + 1);
            function(bitpos + 2);
            break;
          case 0B1000:
            function(bitpos + 3);
            break;
          case 0B1001:
            function(bitpos + 0);
            function(bitpos + 3);
            break;
          case 0B1010:
            function(bitpos + 1);
            function(bitpos + 3);
            break;
          case 0B1011:
            function(bitpos + 0);
            function(bitpos + 1);
            function(bitpos + 3);
            break;
          case 0B1100:
            function(bitpos + 2);
            function(bitpos + 3);
            break;
          case 0B1101:
            function(bitpos + 0);
            function(bitpos + 2);
            function(bitpos + 3);
            break;
          case 0B1110:
            function(bitpos + 1);
            function(bitpos + 2);
            function(bitpos + 3);
            break;
          case 0B1111:
          default:
            function(bitpos + 0);
            function(bitpos + 1);
            function(bitpos + 2);
            function(bitpos + 3);
            break;
        }
      }
    }
  }

  /// \brief Checks if all bits in the bitset are set to 1.
  /// \return Returns true if all bits are 1.
  bool all() const noexcept
  {
    const size_t nw = nof_words_();
    if (nw == 0) {
      return true;
    }
    word_t allset = ~static_cast<word_t>(0);
    for (size_t i = 0; i < nw - 1; i++) {
      if (buffer[i] != allset) {
        return false;
      }
    }
    if constexpr (LowestInfoBitIsMSB) {
      return buffer[nw - 1] == (allset << (nw * bits_per_word - size_()));
    } else {
      return buffer[nw - 1] == (allset >> (nw * bits_per_word - size_()));
    }
  }

  /// \brief Checks if all bits within a bit index range are set to 1.
  /// \return Returns true if all the bits within the range are 1.
  bool all(size_t start, size_t stop) const noexcept
  {
    for (const auto& [word_idx, mask] : make_word_mask_range(start, stop)) {
      if ((buffer[word_idx] | ~mask) != ~static_cast<word_t>(0)) {
        return false;
      }
    }
    return true;
  }

  /// \brief Checks if at least one bit in the bitset is set to 1.
  /// \return Returns true if at least one bit is 1.
  bool any() const noexcept
  {
    for (size_t i = 0, sz = nof_words_(); i != sz; ++i) {
      if (buffer[i] != static_cast<word_t>(0)) {
        return true;
      }
    }
    return false;
  }

  /// \brief Checks if at least one bit in the bitset is set to 1 within a bit index range.
  /// \return Returns true if at least one bit equal to 1 was found within the range.
  bool any(size_t start, size_t stop) const noexcept
  {
    for (const auto& [word_idx, mask] : make_word_mask_range(start, stop)) {
      if ((buffer[word_idx] & mask) != static_cast<word_t>(0)) {
        return true;
      }
    }
    return false;
  }

  /// \brief Checks if at no bit in the bitset is set to 1.
  /// \return Returns true if no bit equal to 1 was found.
  bool none() const noexcept { return !any(); }

  /// \brief Checks whether no bit is set within the given index range.
  /// \return True if no bit is equal to 1, false otherwise.
  bool none(size_t start, size_t stop) const noexcept { return !any(start, stop); }

  /// \brief Checks if all bits set to 1 in this bitset, within a bit index range, are also set to 1 in "other".
  /// \param[in] other Bitset to compare against. Must have the same size as this bitset.
  /// \param[in] start Starting bit index of the range (included).
  /// \param[in] stop End bit index of the range (excluded).
  /// \return Returns true if this bitset restricted to [start, stop) is a subset of "other", false otherwise.
  bool is_subset_of(const Derived& other, size_t start, size_t stop) const noexcept
  {
    ocudu_assert(other.size() == size_(),
                 "ERROR: is_subset_of called for bitsets of different sizes ('{}'!='{}')",
                 size_(),
                 other.size());
    // A bit set in "this" but not in "other" (within the mask) breaks the subset relation.
    for (const auto& [word_idx, mask] : make_word_mask_range(start, stop)) {
      if ((buffer[word_idx] & ~other.buffer[word_idx] & mask) != static_cast<word_t>(0)) {
        return false;
      }
    }
    return true;
  }

  /// \brief Determines whether all bits with value set to \c value are contiguous.
  ///
  /// Bits with the same value are contiguous if:
  /// 1. one bit with \c value is found,
  /// 2. no bit with \c value is found, and
  /// 3. all bits with \c value are consecutive.
  ///
  /// \param[in] value The bit value to find, either true (1) or false (0).
  /// \return Returns true if all the bits set to \c value are contiguous.
  bool is_contiguous(bool value = true) const noexcept
  {
    // Find the lowest value.
    int startpos = find_lowest(0, size_(), value);

    // Condition 1. No value is found.
    if (startpos == -1) {
      return true;
    }

    // Find the highest value.
    int endpos = find_highest(startpos + 1, size_(), value);

    // Condition 2. There is only one bit with the value (in startpos).
    if (endpos == -1) {
      return true;
    }

    // Count the number of elements set to value.
    size_t value_count = count();
    if (not value) {
      value_count = size_() - value_count;
    }

    // Condition 3. The number of elements must match with the start to end number of elements.
    return (value_count == static_cast<size_t>((endpos + 1) - startpos));
  }

  /// \brief Counts the number of bits set to 1.
  /// \return Returns the number of bits set to 1.
  size_t count() const noexcept
  {
    int result = 0;
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      result += count_ones(buffer[i]);
    }
    return result;
  }

  /// \brief Compares two bitsets.
  /// \return Returns true if both bitsets are equal in size and values of bits.
  bool operator==(const Derived& other) const noexcept
  {
    if (size_() != other.size()) {
      return false;
    }
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      if (buffer[i] != other.buffer[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const Derived& other) const noexcept { return not(*this == other); }

  /// \brief Applies bitwise OR operation lhs |= rhs.
  /// \param[in] other Bitset which corresponds to the rhs of the operation.
  /// \return This object updated after the bitwise OR operation.
  Derived& operator|=(const Derived& other) noexcept
  {
    ocudu_assert(other.size() == size_(),
                 "ERROR: operator|= called for bitsets of different sizes ('{}'!='{}')",
                 size_(),
                 other.size());
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      buffer[i] |= other.buffer[i];
    }
    return derived_();
  }

  /// \brief Applies bitwise AND operation lhs &= rhs.
  /// \param[in] other Bitset which corresponds to the rhs of the operation.
  /// \return This object updated after the bitwise AND operation.
  Derived& operator&=(const Derived& other) noexcept
  {
    ocudu_assert(other.size() == size_(),
                 "ERROR: operator&= called for bitsets of different sizes ('{}'!='{}')",
                 size_(),
                 other.size());
    for (size_t i = 0, nw = nof_words_(); i != nw; ++i) {
      buffer[i] &= other.buffer[i];
    }
    return derived_();
  }

  /// \brief Flips values of bits in the bitset.
  /// \return Returns a copy of this object, updated after the flip operation.
  Derived operator~() const noexcept
  {
    Derived ret(derived_());
    ret.flip();
    return ret;
  }

  /// \brief Conversion of the bitset to unsigned integer of 64 bits. If bitset size is larger than 64 bits, an
  /// assertion is triggered.
  /// \return Unsigned integer representation of the bitset.
  uint64_t to_uint64() const noexcept
  {
    ocudu_assert(nof_words_() == 1, "ERROR: cannot convert bitset of size='{}' to uint64_t", size_());
    if constexpr (LowestInfoBitIsMSB) {
      const size_t rem = size_() % bits_per_word;
      return (rem == 0) ? buffer[0] : (buffer[0] >> (bits_per_word - rem));
    }
    return buffer[0];
  }

  /// \brief Conversion of unsigned integer of 64 bits to the bitset. If passed bitmap doesn't fit in the bitset,
  /// an assertion is triggered.
  /// \param[in] v Integer bitmap that is going to be stored in the bitset.
  void from_uint64(uint64_t v) noexcept
  {
    ocudu_assert(nof_words_() == 1, "ERROR: cannot convert bitset of size='{}' to uint64_t", size_());
    ocudu_assert((size_() == 64U) || (v < (static_cast<uint64_t>(1U) << size_())),
                 "ERROR: Provided mask='{}' does not fit in bitset of size='{}'",
                 v,
                 size_());
    if constexpr (LowestInfoBitIsMSB) {
      const size_t rem = size_() % bits_per_word;
      buffer[0]        = (rem == 0) ? v : (v << (bits_per_word - rem));
    } else {
      buffer[0] = v;
    }
  }

  /// \brief Converts the bitset to an array of packed bits. Each element of the resulting array will contain a bitmap.
  /// The LowInfoBitIsMSB template parameter defines the order of bits in the resulting array.
  /// \tparam UnsignedInteger Value type of the array where packed bits will be stored. It must be an unsigned integer.
  /// \param[in] packed_bits Array where packed bits will be stored. The array size must be equal or larger than the
  /// bitset size (in bits) divided by \c sizeof(UnsignedInteger) * 8U (the number of bits per integer).
  /// \return Returns the number of positions of \c packed_bits that were written during the function call.
  template <typename UnsignedInteger>
  size_t to_packed_bits(span<UnsignedInteger> packed_bits) const noexcept
  {
    static_assert(sizeof(UnsignedInteger) <= sizeof(word_t), "ERROR: provided array type is too large");
    static_assert(std::is_unsigned_v<UnsignedInteger>, "Only unsigned integers are supported");
    static constexpr size_t steps_per_word   = sizeof(word_t) / sizeof(UnsignedInteger);
    static constexpr size_t bits_per_integer = sizeof(UnsignedInteger) * 8U;
    static constexpr auto   integer_mask     = mask_lsb_ones<word_t>(bits_per_integer);
    const word_t            sz               = size_();
    const unsigned          last_word_steps =
        (sz % bits_per_word) ? ((sz % bits_per_word) + bits_per_integer - 1) / bits_per_integer : steps_per_word;
    const unsigned nof_words           = nof_words_();
    const unsigned nof_integers_packed = (nof_words - 1) * steps_per_word + last_word_steps;
    ocudu_assert(
        packed_bits.size() >= nof_integers_packed, "ERROR: provided array size='{}' is too small", packed_bits.size());

    size_t count = 0;
    if (not LowestInfoBitIsMSB) {
      for (unsigned i = 0; i != nof_words; ++i) {
        unsigned nof_steps = i == nof_words - 1 ? last_word_steps : steps_per_word;
        for (unsigned j = 0; j != nof_steps; ++j) {
          packed_bits[count++] = (buffer[i] >> (j * steps_per_word)) & integer_mask;
        }
      }
    } else {
      for (unsigned i = 0; i != nof_words; ++i) {
        word_t   w         = buffer[i];
        unsigned nof_steps = steps_per_word;
        if (i == nof_words - 1) {
          nof_steps = last_word_steps;
        }
        for (unsigned j = 0; j != nof_steps; ++j) {
          packed_bits[count++] = (w >> (bits_per_word - (j + 1) * bits_per_integer)) & integer_mask;
        }
      }
    }

    return nof_integers_packed;
  }

  /// \brief Converts the bitset to an array of unpacked bits, i.e. an array where each element represents a single
  /// boolean. The order of bits in the resulting array matches the bit information order in the bitset and the template
  /// parameter \c LowInfoBitIsMSB has no effect. That means that \c unpacked_bits[i] will be equal to \c
  /// bitset.test(i).
  ///
  /// \tparam UnsignedInteger Value type of the array where packed bits will be stored. It must be an
  /// unsigned integer. \param[in] unpacked_bits Array where the unpacked bits will be stored. The array size must be
  /// equal or larger than the bitset size (in bits). \return Returns the number of bits packed.
  template <typename UnsignedInteger>
  void to_unpacked_bits(span<UnsignedInteger> unpacked_bits) const noexcept
  {
    static_assert(std::is_unsigned_v<UnsignedInteger>, "Only unsigned integers are supported");
    ocudu_assert(size_() == unpacked_bits.size(),
                 "ERROR: provided array size='{}' does not match bitset size='{}'",
                 unpacked_bits.size(),
                 size_());

    for (unsigned i = 0, ie = size_(); i != ie; ++i) {
      unpacked_bits[i] = test(i);
    }
  }

  /// \brief Generates a list of bit positions corresponding to the information bits set to one or zero.
  ///
  /// The bit positions correspond to the location of each bit within the information bit word stored in the bitset,
  /// regardless of the bit index order in memory given by \ref LowestInfoBitIsMSB.
  ///
  /// \param[in] value Selects the bits whose positions are returned. Set to \c true to find ones, \c false for zeros.
  /// \return A list containing the bit positions.
  static_vector<size_t, N> get_bit_positions(bool value = true) const noexcept
  {
    static_vector<size_t, N> positions;

    for (size_t i_bit = 0, sz = size_(); i_bit != sz;) {
      // Find the next bit position of the bit set to value.
      int next_position = find_lowest(i_bit, sz, value);
      if (next_position < 0) {
        break;
      }

      // If a bit was found, add to the list.
      positions.emplace_back(static_cast<size_t>(next_position));

      // Exclude the evaluated bit range from the next search.
      i_bit = next_position + 1;
    }

    return positions;
  }

protected:
  // Capacity of the underlying array in number of words.
  static constexpr size_t max_nof_words_() noexcept { return (N + bits_per_word - 1) / bits_per_word; }

  /// Words holding the bits of the bitset.
  std::array<word_t, max_nof_words_()> buffer{};

  /// Number of words currently in use.
  OCUDU_FORCE_INLINE constexpr size_t nof_words_() const noexcept { return bitset_detail::nof_words(size_()); }

  constexpr void assert_within_bounds_(size_t pos, bool strict) const noexcept
  {
    bitset_detail::assert_within_bounds(pos, size_(), strict);
  }

  constexpr void assert_range_bounds_(size_t startpos, size_t endpos) const noexcept
  {
    bitset_detail::assert_range_bounds(startpos, endpos, size_());
  }

  /// \brief Returns a range that enumerates, for a bit interval [start, stop), the indices of the words that
  /// intersect the interval, together with the mask of bits (within each word) that fall inside it.
  ///
  /// \param start first bit index of the bitset.
  /// \param stop end bit index of the bitset.
  auto make_word_mask_range(size_t start, size_t stop) const noexcept
  {
    assert_range_bounds_(start, stop);
    const size_t start_word  = bitset_detail::get_word_idx(start);
    const size_t end_word    = start == stop ? start_word : bitset_detail::get_word_idx(stop - 1) + 1;
    const size_t startmod    = start % bits_per_word;
    const size_t stopmod     = stop % bits_per_word;
    const size_t tail_unused = stopmod == 0 ? 0 : bits_per_word - stopmod;

    return views::transform(
        views::iota(start_word, end_word),
        bitset_detail::word_mask_functor<LowestInfoBitIsMSB>{start_word, end_word, startmod, stopmod, tail_unused});
  }

  OCUDU_FORCE_INLINE static size_t convert_bitpos_(size_t bitpos) noexcept
  {
    if constexpr (LowestInfoBitIsMSB) {
      return bits_per_word - 1 - (bitpos % bits_per_word);
    } else {
      return bitpos;
    }
  }

  OCUDU_FORCE_INLINE static constexpr word_t maskbit(size_t pos) noexcept
  {
    if constexpr (LowestInfoBitIsMSB) {
      return static_cast<word_t>(1U) << (bits_per_word - 1 - (pos % bits_per_word));
    } else {
      return static_cast<word_t>(1U) << (pos % bits_per_word);
    }
  }

  constexpr void sanitize_() noexcept
  {
    const size_t n = size_() % bits_per_word;
    if (n != 0) {
      const size_t nwords = nof_words_();
      if constexpr (LowestInfoBitIsMSB) {
        buffer[nwords - 1] &= ~((~static_cast<word_t>(0)) >> n);
      } else {
        buffer[nwords - 1] &= ~((~static_cast<word_t>(0)) << n);
      }
    }
  }

  OCUDU_FORCE_INLINE constexpr bool test_(size_t bitpos) const noexcept
  {
    const size_t word_idx = bitpos / bits_per_word;
    ocudu_assume(word_idx < buffer.size());
    const word_t bitmask = maskbit(bitpos);
    return ((buffer[word_idx] & bitmask) != static_cast<word_t>(0));
  }

  constexpr void set_(size_t bitpos, bool val) noexcept
  {
    if (val) {
      set_(bitpos);
    } else {
      reset_(bitpos);
    }
  }

  constexpr void set_(size_t bitpos) noexcept
  {
    const size_t word_idx = bitpos / bits_per_word;
    ocudu_assume(word_idx < buffer.size());
    buffer[word_idx] |= maskbit(bitpos);
  }

  constexpr void reset_(size_t bitpos) noexcept
  {
    const size_t word_idx = bitpos / bits_per_word;
    ocudu_assume(word_idx < buffer.size());
    buffer[word_idx] &= ~maskbit(bitpos);
  }

private:
  OCUDU_FORCE_INLINE constexpr size_t size_() const noexcept { return static_cast<const Derived*>(this)->size(); }

  OCUDU_FORCE_INLINE constexpr Derived& derived_() noexcept { return *static_cast<Derived*>(this); }

  OCUDU_FORCE_INLINE constexpr const Derived& derived_() const noexcept { return *static_cast<const Derived*>(this); }
};

/// \brief Executes a function for all \c true (or all \c false) intervals in the given bitset interval.
///
/// \param[in] startpos Smallest bit index considered for the function execution (included).
/// \param[in] endpos   Largest bit index considered for the function execution (excluded).
/// \param[in] function Function to execute - the signature should be compatible with <tt>void ()(size_t,size_t)</tt>.
/// \param[in] value    Bit value that triggers the function execution.
template <typename Bitset, class Func>
void for_each_bitset_interval(const Bitset& bitset, size_t startpos, size_t endpos, Func&& function, bool value)
{
  static_assert(std::is_invocable_v<Func&, size_t, size_t>,
                "The function must be invocable with an interval start and end.");

  // Iterate for all intervals.
  for (int start_interval = bitset.find_lowest(startpos, endpos, value); start_interval != (-1);
       start_interval     = bitset.find_lowest(start_interval, endpos, value)) {
    // Find the end of the current interval.
    int end_interval = bitset.find_lowest(start_interval, endpos, !value);

    // If no more ending, force the end position.
    if (end_interval == -1) {
      end_interval = endpos;
    }

    // Call function.
    function(start_interval, end_interval);

    // Advance interval.
    start_interval = end_interval;
  }
}

} // namespace detail
} // namespace ocudu
