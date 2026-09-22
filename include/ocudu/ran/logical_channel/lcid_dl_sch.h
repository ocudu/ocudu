// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/logical_channel/lcid.h"
#include "ocudu/support/ocudu_assert.h"

namespace ocudu {

/// TS 38.321, Table 6.2.1-1b - Values of one-octet eLCID for DL-SCH.
enum class elcid_dl_sch_t : uint8_t {
  /// [Implementation-defined] Codepoints 0 to 215 are reserved, so none of them identifies a MAC CE.
  INVALID_ELCID        = 0,
  DIFFERENTIAL_KOFFSET = 230
};

/// Number of payload bytes of the MAC CE identified by the provided one-octet eLCID.
constexpr uint32_t sizeof_elcid_ce(elcid_dl_sch_t elcid)
{
  // Values taken from TS 38.321, Section 6.1.3.
  switch (elcid) {
    case elcid_dl_sch_t::DIFFERENTIAL_KOFFSET:
      return 1;
    default:
      return 0;
  }
}

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

    /// Extended logical channel ID field (one-octet eLCID field)
    EXT_LCID_1_OCTET = 0b100010,

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
  /// Returns the LCID of the MAC CE identified by the provided one-octet eLCID.
  static constexpr lcid_dl_sch_t from_elcid(elcid_dl_sch_t elcid_)
  {
    lcid_dl_sch_t lcid{EXT_LCID_1_OCTET};
    lcid.elcid_val = elcid_;
    return lcid;
  }
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
  constexpr bool is_ce() const { return is_elcid() or (lcid_val <= PADDING and lcid_val >= RECOMMENDED_BIT_RATE); }

  /// Whether the MAC CE is identified by a one-octet eLCID rather than by the LCID alone.
  constexpr bool is_elcid() const { return lcid_val == EXT_LCID_1_OCTET; }

  /// eLCID of the MAC CE, as per TS 38.321, Table 6.2.1-1b.
  constexpr elcid_dl_sch_t to_elcid() const
  {
    ocudu_assert(is_elcid(), "Invalid to_elcid() access to lcid={}", lcid_val);
    return elcid_val;
  }

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
    if (is_elcid()) {
      return sizeof_elcid_ce(elcid_val);
    }
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

  constexpr bool operator==(lcid_dl_sch_t other) const
  {
    return lcid_val == other.lcid_val and (not is_elcid() or elcid_val == other.elcid_val);
  }
  constexpr bool operator!=(lcid_dl_sch_t other) const { return not(*this == other); }

private:
  underlying_type lcid_val;
  /// eLCID of the MAC CE. Only meaningful when the LCID is the one-octet eLCID field.
  elcid_dl_sch_t elcid_val = elcid_dl_sch_t::INVALID_ELCID;
};

constexpr uint16_t format_as(lcid_dl_sch_t lcid)
{
  return static_cast<uint16_t>(lcid.value());
}

} // namespace ocudu
