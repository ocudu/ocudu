// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>
#include <variant>

namespace ocudu {

/// \brief Configuration of group and sequence hopping as described in TS38.331 PUCCH-ConfigCommon IE.
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

/// \brief PUCCH Formats as described in TS38.213 Section 9.2.
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
/// TS38.213, Section 9.2.6.
enum class pucch_repetition_tx_slot { no_multi_slot, starts, continues, ends };

/// \brief Identifier of a PUCCH resource.
///
/// Carries either:
///   - a \e dedicated identifier (cell-wide id + UE-specific id) for UE-configured PUCCH resources, or
///   - a \e common identifier (\f$r_{PUCCH}\f$, TS 38.213 Section 9.2.1) for PUCCH resources from the default
///     common table used before dedicated PUCCH is configured.
struct pucch_res_id_t {
  /// Identifier for a PUCCH resource from the default common table (TS 38.213 Table 9.2.1-1).
  struct common {
    /// \f$r_{PUCCH}\f$, range {0, ..., 15}.
    unsigned r_pucch = 0;

    bool operator==(const common& rhs) const { return r_pucch == rhs.r_pucch; }
    bool operator!=(const common& rhs) const { return !(rhs == *this); }
  };
  /// Identifier for a UE-dedicated PUCCH resource.
  struct dedicated {
    /// PUCCH cell resource ID, used by the DU and the scheduler to identify the resource.
    unsigned cell_res_id = 0;
    /// Corresponds to \c pucch-ResourceId in \c PUCCH-Resource, \c PUCCH-Config, TS 38.331.
    /// \remark Used by the DU only to populate the ASN.1 message; not used by the scheduler.
    unsigned ue_res_id = 0;

    bool operator==(const dedicated& rhs) const { return cell_res_id == rhs.cell_res_id && ue_res_id == rhs.ue_res_id; }
    bool operator!=(const dedicated& rhs) const { return !(rhs == *this); }
  };
  std::variant<dedicated, common> id;

  static pucch_res_id_t make_cmn(unsigned r_pucch) { return pucch_res_id_t{common{r_pucch}}; }
  static pucch_res_id_t make_ded(unsigned cell_res_id_, unsigned ue_res_id_)
  {
    return pucch_res_id_t{dedicated{cell_res_id_, ue_res_id_}};
  }

  bool is_cmn() const { return std::holds_alternative<common>(id); }
  bool is_ded() const { return std::holds_alternative<dedicated>(id); }

  const common&    cmn() const { return std::get<common>(id); }
  const dedicated& ded() const { return std::get<dedicated>(id); }

  bool operator==(const pucch_res_id_t& rhs) const { return id == rhs.id; }
  bool operator!=(const pucch_res_id_t& rhs) const { return !(rhs == *this); }
};

} // namespace ocudu
