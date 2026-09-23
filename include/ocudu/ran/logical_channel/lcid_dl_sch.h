// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/logical_channel/lcid.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

/// \brief LCID representation for PDSCH.
class lcid_dl_sch_t
{
  using underlying_type = std::underlying_type_t<lcid_t>;

public:
  /// 3GPP 38.321, Table 6.2.1-1 - Values of LCID for DL-SCH Index
  enum options : underlying_type {
    CCCH = 0b000000,

    /// Identity of the logical channel
    LCID1 = 1,
    // ...
    LCID32 = 32,

    /// Reserved
    MIN_RESERVED = 35,
    MAX_RESERVED = 46,

    /// [Implementation-defined] Marks a subPDU that carries nothing. Reserved by the table, so never a real LCID.
    INVALID = MIN_RESERVED,

    RECOMMENDED_BIT_RATE = 0b101111,

    // TODO: Add remaining.

    SCELL_ACTIV_4_OCTET = 0b111001,
    SCELL_ACTIV_1_OCTET = 0b111010,

    LONG_DRX_CMD  = 0b111011,
    DRX_CMD       = 0b111100,
    TA_CMD        = 0b111101,
    UE_CON_RES_ID = 0b111110,
    PADDING       = 0b111111
  };

  constexpr lcid_dl_sch_t() : lcid_val(PADDING) {}
  constexpr explicit lcid_dl_sch_t(underlying_type lcid_) : lcid_val(lcid_)
  {
    ocudu_assert(lcid_ <= PADDING, "Invalid LCID");
  }
  constexpr lcid_dl_sch_t(lcid_t lcid_) : lcid_val(static_cast<underlying_type>(lcid_))
  {
    ocudu_assert(lcid_val <= PADDING, "Invalid LCID");
  }
  constexpr lcid_dl_sch_t(options lcid_) : lcid_val(lcid_) {}
  constexpr lcid_dl_sch_t& operator=(underlying_type lcid)
  {
    ocudu_assert(lcid <= PADDING, "Invalid LCID");
    lcid_val = lcid;
    return *this;
  }

  /// convert lcid_dl_sch_t to underlying integer type via cast.
  explicit constexpr operator underlying_type() const { return lcid_val; }

  /// convert lcid_dl_sch_t to underlying integer type.
  constexpr underlying_type value() const { return lcid_val; }

  /// Whether LCID is an MAC CE
  constexpr bool is_ce() const { return lcid_val <= PADDING and lcid_val >= RECOMMENDED_BIT_RATE; }

  /// Whether LCID belongs to a Radio Bearer Logical Channel
  constexpr bool is_sdu() const { return lcid_val <= LCID32 and lcid_val >= CCCH; }

  constexpr lcid_t to_lcid() const
  {
    ocudu_assert(is_sdu(), "Invalid to_lcid() access to lcid={}", lcid_val);
    return (lcid_t)lcid_val;
  }

  /// Returns false for all reserved values in Table 6.2.1-1 and 6.2.1-2
  constexpr bool is_valid() const
  {
    return lcid_val <= PADDING and (lcid_val < MIN_RESERVED or lcid_val > MAX_RESERVED);
  }

  constexpr bool is_var_len_ce() const { return false; }

  constexpr uint32_t sizeof_ce() const
  {
    // Values taken from TS38.321, Section 6.1.3.
    switch (lcid_val) {
      case SCELL_ACTIV_4_OCTET:
        return 4;
      case SCELL_ACTIV_1_OCTET:
        return 1;
      case LONG_DRX_CMD:
      case DRX_CMD:
        return 0;
      case TA_CMD:
        return 1;
      case UE_CON_RES_ID:
        return 6;
      case PADDING:
        return 0;
      default:
        break;
    }
    return 0;
  }

  constexpr bool operator==(lcid_dl_sch_t other) const { return lcid_val == other.lcid_val; }
  constexpr bool operator!=(lcid_dl_sch_t other) const { return lcid_val != other.lcid_val; }

private:
  underlying_type lcid_val;
};

constexpr uint16_t format_as(lcid_dl_sch_t lcid)
{
  return static_cast<uint16_t>(lcid.value());
}

} // namespace ocudu
