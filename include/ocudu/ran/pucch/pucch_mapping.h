// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/support/ocudu_assert.h"
#include "fmt/base.h"
#include <cstdint>
#include <limits>

namespace ocudu {

/// \brief Configuration of group and sequence hopping as described in TS 38.331 PUCCH-ConfigCommon IE.
enum class pucch_group_hopping {
  /// Implies neither group or sequence hopping is enabled.
  NEITHER,
  /// Enables group hopping and disables sequence hopping.
  ENABLE,
  /// Disables group hopping and enables sequence hopping.
  DISABLE
};

/// \brief PUCCH Resource Set ID, as per \c PUCCH-ResourceSetId, TS 38.331.
///
/// At the moment, we only supports PUCCH resource set index 0 and 1.
enum class pucch_res_set_id : uint8_t { set_0 = 0, set_1 };

inline uint8_t to_uint(pucch_res_set_id format)
{
  return static_cast<uint8_t>(format);
}

/// \brief PUCCH Formats as described in TS 38.213 Section 9.2.
enum class pucch_format : uint8_t { FORMAT_0, FORMAT_1, FORMAT_2, FORMAT_3, FORMAT_4, NOF_FORMATS };

inline uint8_t to_uint(pucch_format format)
{
  return static_cast<uint8_t>(format);
}

inline const char* to_string(pucch_format format)
{
  switch (format) {
    case pucch_format::FORMAT_0:
      return "FORMAT_0";
    case pucch_format::FORMAT_1:
      return "FORMAT_1";
    case pucch_format::FORMAT_2:
      return "FORMAT_2";
    case pucch_format::FORMAT_3:
      return "FORMAT_3";
    case pucch_format::FORMAT_4:
      return "FORMAT_4";
    default:
      return "UNKNOWN";
  }
}

/// Defines the allowed combinations of PUCCH formats to use for the resources in a given cell.
enum class pucch_formats { f0_and_f2, f1_and_f2, f1_and_f3, f1_and_f4 };

inline const char* to_string(pucch_formats formats)
{
  switch (formats) {
    case pucch_formats::f0_and_f2:
      return "f0_and_f2";
    case pucch_formats::f1_and_f2:
      return "f1_and_f2";
    case pucch_formats::f1_and_f3:
      return "f1_and_f3";
    case pucch_formats::f1_and_f4:
      return "f1_and_f4";
    default:
      return "UNKNOWN";
  }
}

/// \brief Returns a format between F0 and F1 for the given combination of PUCCH formats.
/// This format is used for Resource Set ID 0 and SR resources.
inline pucch_format pucch_f0f1_format(pucch_formats formats)
{
  switch (formats) {
    case pucch_formats::f0_and_f2:
      return pucch_format::FORMAT_0;
    case pucch_formats::f1_and_f2:
    case pucch_formats::f1_and_f3:
    case pucch_formats::f1_and_f4:
      return pucch_format::FORMAT_1;
    default:
      return pucch_format::NOF_FORMATS;
  }
}

/// Returns a format between F2, F3 and F4 for the given combination of PUCCH formats.
/// This format is used for Resource Set ID 1 and CSI resources.
inline pucch_format pucch_f2f3f4_format(pucch_formats formats)
{
  switch (formats) {
    case pucch_formats::f0_and_f2:
    case pucch_formats::f1_and_f2:
      return pucch_format::FORMAT_2;
    case pucch_formats::f1_and_f3:
      return pucch_format::FORMAT_3;
    case pucch_formats::f1_and_f4:
      return pucch_format::FORMAT_4;
    default:
      return pucch_format::NOF_FORMATS;
  }
}

/// Defines whether the PUCCH within the current slot belongs to a PUCCH repetition. For more details, refer to
/// TS 38.213, Section 9.2.6.
enum class pucch_repetition_tx_slot { no_multi_slot, starts, continues, ends };

/// \brief Identifier of a PUCCH resource, either common or dedicated, in a single index space.
///
/// The index space has two ranges:
///   - [0, 15]: common resources. The index is \f$r_{PUCCH}\f$ of TS 38.213 Table 9.2.1-1.
///   - [16, ...): dedicated resources. The index is 16 plus the dedicated resource ID (\c ded_id), i.e. its position
///     in the cell list of dedicated PUCCH resources (0 for the first one).
///
/// A default-constructed ID is invalid. Use \c make_cmn, \c make_ded or \c from_idx to create a valid one.
class pucch_res_id_t
{
public:
  pucch_res_id_t() = default;

  /// Number of common PUCCH resources, i.e. the number of rows of TS 38.213 Table 9.2.1-1.
  static constexpr unsigned nof_cmn_res = pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES;

  static pucch_res_id_t make_cmn(unsigned r_pucch)
  {
    ocudu_assert(r_pucch < nof_cmn_res, "Invalid r_pucch={} for a common PUCCH resource", r_pucch);
    return pucch_res_id_t{r_pucch};
  }
  static pucch_res_id_t make_ded(unsigned ded_id)
  {
    ocudu_assert(ded_id < pucch_constants::MAX_NOF_CELL_DED_RESOURCES,
                 "Invalid ded_id={} for a dedicated PUCCH resource",
                 ded_id);
    return pucch_res_id_t{nof_cmn_res + ded_id};
  }
  /// Creates the ID from an index over the common and the dedicated resources, see \c value.
  static pucch_res_id_t from_idx(unsigned idx)
  {
    ocudu_assert(
        idx < nof_cmn_res + pucch_constants::MAX_NOF_CELL_DED_RESOURCES, "Invalid PUCCH resource index={}", idx);
    return pucch_res_id_t{idx};
  }

  /// Whether the ID identifies a resource, i.e. it is not default-constructed.
  bool is_valid() const { return idx != invalid_idx; }

  /// Returns the index over the common resources followed by the dedicated resources.
  unsigned value() const
  {
    ocudu_assert(is_valid(), "Invalid PUCCH resource ID");
    return idx;
  }

  bool is_cmn() const
  {
    ocudu_assert(is_valid(), "Invalid PUCCH resource ID");
    return idx < nof_cmn_res;
  }
  bool is_ded() const { return not is_cmn(); }

  /// Returns \f$r_{PUCCH}\f$ of a common resource.
  unsigned r_pucch() const
  {
    ocudu_assert(is_cmn(), "PUCCH resource ID={} is not common", idx);
    return idx;
  }
  /// Returns the ID of a dedicated resource, i.e. its position in the cell list of dedicated PUCCH resources.
  unsigned ded_id() const
  {
    ocudu_assert(is_ded(), "PUCCH resource ID={} is not dedicated", idx);
    return idx - nof_cmn_res;
  }

  bool operator==(const pucch_res_id_t& rhs) const { return idx == rhs.idx; }
  bool operator!=(const pucch_res_id_t& rhs) const { return !(rhs == *this); }

private:
  explicit pucch_res_id_t(unsigned idx_) : idx(static_cast<uint16_t>(idx_)) {}

  static constexpr uint16_t invalid_idx = std::numeric_limits<uint16_t>::max();

  uint16_t idx = invalid_idx;
};

} // namespace ocudu

// Formatters
namespace fmt {
template <>
struct formatter<ocudu::pucch_res_id_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::pucch_res_id_t res_id, FormatContext& ctx) const
  {
    if (not res_id.is_valid()) {
      return format_to(ctx.out(), "invalid");
    }
    if (res_id.is_cmn()) {
      return format_to(ctx.out(), "cmn({})", res_id.r_pucch());
    }
    return format_to(ctx.out(), "ded({})", res_id.ded_id());
  }
};

} // namespace fmt
