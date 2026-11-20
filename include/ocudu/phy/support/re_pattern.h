// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/format.h"
#include "ocudu/adt/interval.h"
#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/phy/support/mask_types.h"
#include "ocudu/ran/resource_allocation/rb_bitmap.h"
#include <utility>

namespace ocudu {

/// Describes a resource element pattern within a resource grid.
struct re_pattern {
  /// Common Resource Block (CRB) mask, relative to Point A.
  crb_bitmap crb_mask;
  /// Resource element mask per resource block. True entries indicate the resource elements affected by the pattern.
  re_prb_mask re_mask;
  /// Symbol mask. True entries indicate the symbols affected by the pattern.
  symbol_slot_mask symbols;

  /// Creates a pattern with no CRBs, resource elements or symbols.
  re_pattern() = default;

  /// \brief Constructs an RE pattern from a CRB range with a stride.
  ///
  /// The pattern contains the CRBs \c rb_begin, \c rb_begin + \c rb_stride, ... below \c rb_end. The CRB mask has
  /// \c rb_end entries.
  ///
  /// \param[in] rb_begin  First CRB index, within {0, ..., MAX_NOF_PRBS - 1}.
  /// \param[in] rb_end    CRB index after the last CRB, within {1, ..., MAX_NOF_PRBS}.
  /// \param[in] rb_stride CRB index step, within {1, ..., MAX_NOF_PRBS - 1}.
  /// \param[in] re_mask_  Resource element mask per CRB.
  /// \param[in] symbols_  OFDM symbol mask.
  /// \remark An assertion is triggered if \c rb_begin, \c rb_end or \c rb_stride is outside its range.
  /// \remark An assertion is triggered if \c rb_begin is not lower than \c rb_end.
  re_pattern(unsigned rb_begin, unsigned rb_end, unsigned rb_stride, re_prb_mask re_mask_, symbol_slot_mask symbols_) :
    crb_mask(rb_end), re_mask(std::move(re_mask_)), symbols(std::move(symbols_))
  {
    static constexpr interval<unsigned, true> rb_begin_interval  = {0, MAX_NOF_PRBS - 1};
    static constexpr interval<unsigned, true> rb_stride_interval = {1, MAX_NOF_PRBS - 1};
    static constexpr interval<unsigned, true> rb_end_interval    = {1, MAX_NOF_PRBS};

    ocudu_assert(rb_begin_interval.contains(rb_begin),
                 "The RB begin (i.e., {}) is out of range {}.",
                 rb_begin,
                 rb_begin_interval);
    ocudu_assert(
        rb_end_interval.contains(rb_end), "The RB end (i.e., {}) is out of range {}.", rb_end, rb_end_interval);
    ocudu_assert(rb_stride_interval.contains(rb_stride),
                 "The RB stride (i.e., {}) is out of range {}.",
                 rb_stride,
                 rb_stride_interval);
    ocudu_assert(rb_begin < rb_end, "The RB begin (i.e., {}) must be smaller than RB end (i.e., {})", rb_begin, rb_end);

    if (rb_stride == 1) {
      crb_mask.fill(rb_begin, rb_end);
    } else {
      for (unsigned i_prb = rb_begin; i_prb < rb_end; i_prb += rb_stride) {
        crb_mask.set(i_prb);
      }
    }
  }

  /// \brief Copy constructor.
  ///
  /// \param[in] other Provides the reference to other resource element pattern to copy.
  re_pattern(const re_pattern& other) : crb_mask(other.crb_mask), re_mask(other.re_mask), symbols(other.symbols)
  {
    // Do nothing.
  }

  /// \brief Sets the resource elements of the pattern in an OFDM symbol mask.
  ///
  /// The mask has one entry per subcarrier, starting at Point A, and any size. The method ignores the pattern resource
  /// elements beyond the mask size. It leaves the mask unchanged if the pattern does not use \c symbol.
  ///
  /// \param[in,out] mask   Resource element mask of one OFDM symbol.
  /// \param[in]     symbol OFDM symbol index within the slot.
  /// \remark An assertion is triggered if \c symbol is not lower than the size of \c symbols.
  /// \remark An assertion is triggered if the pattern uses \c symbol and \c crb_mask has no CRBs.
  void get_inclusion_mask(bounded_bitset<MAX_NOF_SUBCARRIERS>& mask, unsigned symbol) const;

  /// \brief Clears the resource elements of the pattern in an OFDM symbol mask.
  ///
  /// The mask has one entry per subcarrier, starting at Point A, and any size. The method ignores the pattern resource
  /// elements beyond the mask size. It leaves the mask unchanged if the pattern does not use \c symbol.
  ///
  /// \param[in,out] mask   Resource element mask of one OFDM symbol.
  /// \param[in]     symbol OFDM symbol index within the slot.
  /// \remark An assertion is triggered if \c symbol is not lower than the size of \c symbols.
  /// \remark An assertion is triggered if the pattern uses \c symbol and \c crb_mask has no CRBs.
  void get_exclusion_mask(bounded_bitset<MAX_NOF_SUBCARRIERS>& mask, unsigned symbol) const;
};

/// \brief Checks if two RE patterns are equal.
///
/// Two RE patterns are equal if their attributes are equal.
inline bool operator==(const re_pattern& pattern1, const re_pattern& pattern2)
{
  if (pattern1.symbols != pattern2.symbols) {
    return false;
  }

  if (pattern1.crb_mask != pattern2.crb_mask) {
    return false;
  }

  if (pattern1.re_mask != pattern2.re_mask) {
    return false;
  }

  return true;
}

/// Describes a resource element pattern list.
class re_pattern_list
{
private:
  /// Maximum number of RE patterns in a list.
  static constexpr unsigned MAX_RE_PATTERN = 4;

  /// Resource element pattern storage.
  static_vector<re_pattern, MAX_RE_PATTERN> list;

public:
  /// Default constructor.
  re_pattern_list() = default;

  /// Implicit construction from a single RE pattern.
  re_pattern_list(const re_pattern& pattern) { list.emplace_back(pattern); }

  /// \brief Creates a pattern list by merging the patterns of an initializer list.
  ///
  /// It suits static constant pattern lists. For example:
  /// \code
  ///   re_pattern_list list = {
  ///      {0, 52, 1, {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}},
  ///      {0, 52, 1, {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}}};
  /// \endcode
  ///
  /// \param[in] patterns Initializer list with patterns.
  /// \remark An assertion is triggered if the merged patterns need more than \c MAX_RE_PATTERN entries.
  re_pattern_list(std::initializer_list<const re_pattern> patterns)
  {
    for (const re_pattern& pattern : patterns) {
      merge(pattern);
    }
  }

  /// \brief Copy constructor.
  ///
  /// \param[in] other Provides other instance.
  re_pattern_list(const re_pattern_list& other) : list(other.list)
  {
    // Do nothing.
  }

  /// Removes all the patterns from the list.
  void clear() { list.clear(); }

  /// Returns the number of patterns in the list.
  unsigned get_nof_entries() const { return list.size(); }

  /// \brief Merges the patterns of another pattern list into the list, one by one.
  /// \param[in] other Pattern list to merge.
  /// \remark An assertion is triggered if the merged patterns need more than \c MAX_RE_PATTERN entries.
  void merge(const re_pattern_list& other)
  {
    for (const auto& pattern : other.get_re_patterns()) {
      merge(pattern);
    }
  }

  /// \brief Merges a resource element pattern into the list.
  ///
  /// The method combines \c pattern with the first pattern in the list that has at least two equal masks:
  /// - all masks equal: the list already contains \c pattern;
  /// - symbol and resource element masks equal: the CRB masks combine;
  /// - CRB and symbol masks equal: the resource element masks combine;
  /// - CRB and resource element masks equal: the symbol masks combine.
  ///
  /// Otherwise, it appends \c pattern to the list.
  ///
  /// \param[in] pattern Resource element pattern to merge.
  /// \remark An assertion is triggered if the list has \c MAX_RE_PATTERN entries and \c pattern does not combine with
  /// any of them.
  void merge(const re_pattern& pattern);

  /// \brief Checks if the pattern list is equal to another pattern list.
  ///
  /// \remark Two pattern lists are considered equal if their generated inclusion masks are equal for the maximum number
  /// of RBs and symbols.
  /// \param[in] other The other pattern list.
  /// \return True if the patterns described by the two lists are equal.
  /// \remark The assertions of re_pattern::get_inclusion_mask() apply to every pattern of both lists.
  bool operator==(const re_pattern_list& other) const;

  /// \brief Checks if the pattern list is different from another pattern list.
  ///
  /// \param[in] other The other pattern list.
  /// \return True if the patterns described by the two lists are different.
  bool operator!=(const re_pattern_list& other) const { return !(*this == other); }

  /// \brief Sets the resource elements of all the patterns in an OFDM symbol mask.
  ///
  /// The mask has one entry per subcarrier, starting at Point A, and any size. The method ignores the pattern resource
  /// elements beyond the mask size.
  ///
  /// \param[in,out] mask   Resource element mask of one OFDM symbol.
  /// \param[in]     symbol OFDM symbol index within the slot.
  /// \remark The assertions of re_pattern::get_inclusion_mask() apply to every pattern.
  void get_inclusion_mask(bounded_bitset<MAX_NOF_SUBCARRIERS>& mask, unsigned symbol) const;

  /// \brief Counts the resource elements of the pattern list within a symbol range and a CRB selection.
  ///
  /// The method counts each resource element once, even if several patterns contain it.
  ///
  /// \param[in] start_symbol First OFDM symbol index within the slot.
  /// \param[in] nof_symbols  Number of OFDM symbols.
  /// \param[in] crb_mask     Selection of CRBs to count.
  /// \return The number of resource elements within the symbols {start_symbol, ..., start_symbol + nof_symbols - 1}
  /// and the CRBs of \c crb_mask.
  /// \remark The symbol range is within the symbol mask of every pattern.
  /// \remark With several patterns, the assertions of re_pattern::get_inclusion_mask() apply to every pattern.
  unsigned get_inclusion_count(unsigned start_symbol, unsigned nof_symbols, const crb_bitmap& crb_mask) const;

  /// \brief Clears the resource elements of all the patterns in an OFDM symbol mask.
  ///
  /// The mask has one entry per subcarrier, starting at Point A, and any size. The method ignores the pattern resource
  /// elements beyond the mask size.
  ///
  /// \param[in,out] mask   Resource element mask of one OFDM symbol.
  /// \param[in]     symbol OFDM symbol index within the slot.
  /// \remark The assertions of re_pattern::get_exclusion_mask() apply to every pattern.
  void get_exclusion_mask(bounded_bitset<MAX_NOF_SUBCARRIERS>& mask, unsigned symbol) const;

  /// Returns the resource element patterns of the list.
  span<const re_pattern> get_re_patterns() const { return list; }
};

} // namespace ocudu
