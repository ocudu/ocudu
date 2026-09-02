// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include <cstdint>
#include <initializer_list>

namespace ocudu {

enum class sib_type : uint8_t {
  sib1        = 1,
  sib2        = 2,
  sib3        = 3,
  sib4        = 4,
  sib5        = 5,
  sib6        = 6,
  sib7        = 7,
  sib8        = 8,
  sib16       = 16,
  sib19       = 19,
  sib_invalid = 255
};

/// Formats a SIB type as its number.
constexpr unsigned format_as(sib_type sib)
{
  return static_cast<unsigned>(sib);
}

/// \brief Maximum number of SI messages that can carry PWS (ETWS/CMAS) content.
///
/// There are only three PWS SIBs (SIB6, SIB7 and SIB8), and a SIB is mapped to at most one SI message.
constexpr size_t MAX_PWS_SI_MESSAGES = 3;

/// Whether the given SIB carries PWS (ETWS/CMAS) content.
constexpr bool is_pws_sib(sib_type sib)
{
  return sib == sib_type::sib6 or sib == sib_type::sib7 or sib == sib_type::sib8;
}

/// \brief Set of SIBs carried by a single SI message.
///
/// A SIB is mapped to at most one SI message, so this set identifies an SI message within a cell, independently of
/// the position it occupies in the SI scheduling configuration.
class sib_type_set
{
  /// Number of SIB types that fit in the set.
  static constexpr unsigned nof_sib_types = 32;

public:
  sib_type_set() = default;
  sib_type_set(std::initializer_list<sib_type> sibs)
  {
    for (sib_type sib : sibs) {
      add(sib);
    }
  }
  template <typename It>
  sib_type_set(It begin, It end)
  {
    for (It it = begin; it != end; ++it) {
      add(*it);
    }
  }

  void add(sib_type sib)
  {
    ocudu_assert(static_cast<unsigned>(sib) < nof_sib_types, "Invalid SIB type {}", static_cast<unsigned>(sib));
    bitmap.set(static_cast<unsigned>(sib));
  }

  bool contains(sib_type sib) const
  {
    return static_cast<unsigned>(sib) < nof_sib_types and bitmap.test(static_cast<unsigned>(sib));
  }

  bool empty() const { return bitmap.none(); }

  /// Number of SIBs in the set.
  unsigned size() const { return bitmap.count(); }

  /// Lowest SIB in the set. The set must not be empty.
  sib_type front() const
  {
    ocudu_assert(not empty(), "Empty SIB set");
    return static_cast<sib_type>(bitmap.find_lowest());
  }

  /// Invokes the given callable once per SIB in the set, in ascending SIB type order.
  template <typename Callable>
  void for_each(Callable&& func) const
  {
    bitmap.for_each(0, bitmap.size(), [&func](size_t pos) { func(static_cast<sib_type>(pos)); });
  }

  /// Whether this SI message carries a PWS (ETWS/CMAS) SIB, and therefore is only broadcast while a warning is active.
  bool is_pws() const { return contains(sib_type::sib6) or contains(sib_type::sib7) or contains(sib_type::sib8); }

  /// \brief Whether this SI message carries the NTN SIB19.
  ///
  /// Its content is pushed to the PHY immediately, bypassing the SI change modification window, as per TS 38.331,
  /// 5.2.2.2.2 and the \c ntn-Config field descriptions.
  bool is_ntn() const { return contains(sib_type::sib19); }

  bool operator==(const sib_type_set& other) const { return bitmap == other.bitmap; }
  bool operator!=(const sib_type_set& other) const { return bitmap != other.bitmap; }

private:
  bounded_bitset<nof_sib_types> bitmap = bounded_bitset<nof_sib_types>(nof_sib_types);
};

} // namespace ocudu
