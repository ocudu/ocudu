// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/detail/bitset_base.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

/// \brief Represents a bitset with a compile-time fixed size of N bits.
///
/// Contrary to \c bounded_bitset, this class does not store the current bitset size, which is always equal to N.
///
/// \tparam N Size of the bitset in number of bits.
/// \tparam LowestInfoBitIsMSB Bit index order in memory. If set to (false / true), the bit index 0 (Lowest Information
/// Bit) corresponds to either the LSB or MSB of the bitset. Note that this argument has an effect on the underlying
/// bitset memory layout.
template <size_t N, bool LowestInfoBitIsMSB = false>
class fixed_size_bitset : public detail::bitset_crtp<fixed_size_bitset<N, LowestInfoBitIsMSB>, N, LowestInfoBitIsMSB>
{
  using base_t = detail::bitset_crtp<fixed_size_bitset<N, LowestInfoBitIsMSB>, N, LowestInfoBitIsMSB>;

public:
  using base_t::bit_order;
  using base_t::bits_per_word;
  using base_t::empty;
  using base_t::max_size;

  constexpr fixed_size_bitset() = default;

  /// \brief Constructs a bitset using iterators.
  ///
  /// The iterator range must hold exactly N elements, which are mapped one to one starting from begin.
  ///
  /// \tparam Iterator Boolean iterator type.
  /// \param[in] begin Begin iterator.
  /// \param[in] end End iterator.
  template <typename Iterator,
            std::enable_if_t<std::is_convertible_v<typename std::iterator_traits<Iterator>::value_type, bool>, int> = 0>
  constexpr fixed_size_bitset(Iterator begin, Iterator end)
  {
    report_fatal_error_if_not(static_cast<size_t>(end - begin) == N,
                              "ERROR: the fixed_size_bitset iterator range must hold exactly '{}' elements",
                              N);
    auto it = begin;
    for (size_t count = 0; count != N; ++count) {
      this->set_(count, *it);
      ++it;
    }
  }

  /// \brief Constructs a bitset from an initializer list.
  ///
  /// The initializer list must hold exactly N elements, which are mapped one to one.
  ///
  /// \param[in] values Boolean initializer list.
  constexpr fixed_size_bitset(const std::initializer_list<const bool>& values) noexcept
  {
    report_fatal_error_if_not(
        values.size() == N, "ERROR: the fixed_size_bitset initializer list must hold exactly '{}' elements", N);
    auto it = values.begin();
    for (size_t count = 0; count != N; ++count) {
      this->set_(count, *it);
      ++it;
    }
  }

  /// Size of the fixed_size_bitset in bits.
  static constexpr size_t size() noexcept { return N; }
};

/// \brief Bitwise AND operation result = lhs & rhs.
/// \return new fixed_size_bitset that results from the Bitwise AND operation.
template <size_t N, bool LowestInfoBitIsMSB>
inline fixed_size_bitset<N, LowestInfoBitIsMSB> operator&(const fixed_size_bitset<N, LowestInfoBitIsMSB>& lhs,
                                                          const fixed_size_bitset<N, LowestInfoBitIsMSB>& rhs) noexcept
{
  fixed_size_bitset<N, LowestInfoBitIsMSB> res(lhs);
  res &= rhs;
  return res;
}

/// \brief Bitwise OR operation result = lhs | rhs.
/// \return new fixed_size_bitset that results from the Bitwise OR operation.
template <size_t N, bool LowestInfoBitIsMSB>
inline fixed_size_bitset<N, LowestInfoBitIsMSB> operator|(const fixed_size_bitset<N, LowestInfoBitIsMSB>& lhs,
                                                          const fixed_size_bitset<N, LowestInfoBitIsMSB>& rhs) noexcept
{
  fixed_size_bitset<N, LowestInfoBitIsMSB> res(lhs);
  res |= rhs;
  return res;
}

/// \brief Executes a function for all \c true (or all \c false) intervals in the given bitset interval.
///
/// The method calls \c function for each interval, passing the first and last bit positions of the interval.
///
/// \param[in] startpos Smallest bit index considered for the function execution (included).
/// \param[in] endpos   Largest bit index considered for the function execution (excluded).
/// \param[in] function Function to execute - the signature should be compatible with <tt>void ()(size_t,size_t)</tt>.
/// \param[in] value    Bit value that triggers the function execution.
template <size_t N, bool LowestInfoBitIsMSB, class Func>
void for_each_interval(const fixed_size_bitset<N, LowestInfoBitIsMSB>& bitset,
                       size_t                                          startpos,
                       size_t                                          endpos,
                       Func&&                                          function,
                       bool                                            value = true)
{
  detail::for_each_bitset_interval(bitset, startpos, endpos, std::forward<Func>(function), value);
}

// Executes a function for all \c true (or all \c false) intervals in the given bitset.
template <size_t N, bool LowestInfoBitIsMSB, class Func>
void for_each_interval(const fixed_size_bitset<N, LowestInfoBitIsMSB>& bitset, Func&& function, bool value = true)
{
  for_each_interval(bitset, 0, bitset.size(), function, value);
}

} // namespace ocudu
