// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/logical_channel/lcid.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

class lcid_ul_sch_t
{
  using underlying_type = std::underlying_type_t<lcid_t>;

public:
  /// UL-SCH LCID values, as per TS 38.321 v17.13.0, Table 6.2.1-2.
  enum options : underlying_type {
    /// CCCH of 64 bits
    CCCH_SIZE_64 = 0b000000,

    /// Identity of the logical channel
    LCID1 = 1,
    // ...
    LCID32 = 32,

    /// Unsupported range, except for RedCap CCCH and Timing Advance Report.
    MIN_UNSUPPORTED = 33,
    MAX_UNSUPPORTED = 51,

    /// RedCap CCCH of 48 bits.
    CCCH_SIZE_48_REDCAP = 35,
    /// RedCap CCCH of 64 bits.
    CCCH_SIZE_64_REDCAP = 36,

    /// Timing Advance Report (44), see TS 38.321, 6.1.3.56.
    TIMING_ADVANCE_REPORT = 0b101100,

    /// CCCH of 48 bits
    CCCH_SIZE_48 = 0b110100,

    BIT_RATE_QUERY = 0b110101,

    SE_PHR          = 0b111001, // Single Entry PHR
    CRNTI           = 0b111010,
    SHORT_TRUNC_BSR = 0b111011,
    LONG_TRUNC_BSR  = 0b111100,
    SHORT_BSR       = 0b111101,
    LONG_BSR        = 0b111110,
    PADDING         = 0b111111
  };

  lcid_ul_sch_t() : lcid_val(PADDING) {}
  explicit lcid_ul_sch_t(underlying_type lcid_) : lcid_val(lcid_) { ocudu_assert(lcid_ <= PADDING, "Invalid LCID"); }
  lcid_ul_sch_t(lcid_t lcid_) : lcid_val(static_cast<underlying_type>(lcid_))
  {
    ocudu_assert(lcid_val <= PADDING, "Invalid LCID");
  }
  lcid_ul_sch_t(options lcid_) : lcid_val(lcid_) {}
  lcid_ul_sch_t& operator=(underlying_type lcid)
  {
    ocudu_assert(lcid <= PADDING, "Invalid LCID");
    lcid_val = lcid;
    return *this;
  }

  explicit        operator underlying_type() const { return lcid_val; }
  underlying_type value() const { return lcid_val; }

  /// Whether LCID belongs to CCCH
  bool is_ccch() const { return lcid_val == CCCH_SIZE_48 || lcid_val == CCCH_SIZE_64 || is_redcap_ccch(); }

  /// Returns whether the LCID identifies RedCap UL CCCH.
  bool is_redcap_ccch() const { return lcid_val == CCCH_SIZE_48_REDCAP || lcid_val == CCCH_SIZE_64_REDCAP; }

  /// Whether LCID is an MAC CE
  bool is_ce() const
  {
    // The MAC CE codepoints are contiguous from BIT_RATE_QUERY upwards, except for the Timing Advance Report, which
    // sits at 44 among the unsupported codepoints.
    return (lcid_val <= PADDING and lcid_val >= BIT_RATE_QUERY) or lcid_val == TIMING_ADVANCE_REPORT;
  }

  /// Whether LCID belongs to a Radio Bearer Logical Channel
  bool is_sdu() const { return lcid_val <= LCID32 and lcid_val >= LCID1; }

  /// Returns false for the LCID values this MAC does not accept, see \c MIN_UNSUPPORTED.
  bool is_valid_lcid() const
  {
    return is_redcap_ccch() or lcid_val == TIMING_ADVANCE_REPORT or
           (lcid_val <= PADDING and (lcid_val < MIN_UNSUPPORTED or lcid_val > MAX_UNSUPPORTED));
  }

  /// Whether LCID subPDU has associated length field
  bool has_length_field() const
  {
    // Ordinary and RedCap UL CCCH have fixed payload sizes, as per TS 38.321, Section 6.1.2.
    if (is_ccch()) {
      return false;
    }
    return (is_sdu() || is_var_len_ce());
  }

  bool is_var_len_ce() const
  {
    switch (lcid_val) {
      case LONG_TRUNC_BSR:
      case LONG_BSR:
        return true;
      default:
        return false;
    }
  }

  uint32_t sizeof_ce() const
  {
    switch (lcid_val) {
      case CCCH_SIZE_48:
      case CCCH_SIZE_48_REDCAP:
        return 6;
      case CCCH_SIZE_64:
      case CCCH_SIZE_64_REDCAP:
        return 8;
      case CRNTI:
        return 2;
      case SHORT_BSR:
      case SHORT_TRUNC_BSR:
        return 1;
      case SE_PHR:
        return 2;
      case TIMING_ADVANCE_REPORT:
        return 2;
      // NOTE: LONG_BSR and LONG_TRUNC_BSR are variable-sized MAC CE, not fixed-sized. Right now this function is not
      // called for these two cases.
      // TODO: Verify if these two cases will be used in the future by this function.
      case LONG_BSR:
      case LONG_TRUNC_BSR:
        return 1; // minimum size, could be more than that
      case PADDING:
        return 0;
      default:
        break;
    }
    return 0;
  }

  bool operator==(lcid_ul_sch_t other) const { return lcid_val == other.lcid_val; }
  bool operator!=(lcid_ul_sch_t other) const { return lcid_val != other.lcid_val; }

private:
  underlying_type lcid_val;
};

inline uint16_t format_as(lcid_ul_sch_t lcid)
{
  return static_cast<uint16_t>(lcid.value());
}

} // namespace ocudu
