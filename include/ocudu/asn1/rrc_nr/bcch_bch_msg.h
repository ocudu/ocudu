// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v18.8.0 (2025-12)
 *
 ******************************************************************************/

#pragma once

#include "ocudu/asn1/asn1_rrc_utils.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// PDCCH-ConfigSIB1 ::= SEQUENCE
struct pdcch_cfg_sib1_s {
  uint8_t coreset_zero      = 0;
  uint8_t search_space_zero = 0;

  static const asn1::table::seq_desc desc;
};

// MIB ::= SEQUENCE
struct mib_s {
  struct sub_carrier_spacing_common_opts {
    enum options { scs15or60, scs30or120, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using sub_carrier_spacing_common_e_ = enumerated<sub_carrier_spacing_common_opts>;
  struct dmrs_type_a_position_opts {
    enum options { pos2, pos3, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using dmrs_type_a_position_e_ = enumerated<dmrs_type_a_position_opts>;
  struct cell_barred_opts {
    enum options { barred, not_barred, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using cell_barred_e_ = enumerated<cell_barred_opts>;
  struct intra_freq_resel_opts {
    enum options { allowed, not_allowed, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using intra_freq_resel_e_ = enumerated<intra_freq_resel_opts>;

  // member variables
  fixed_bitstring<6>            sys_frame_num;
  sub_carrier_spacing_common_e_ sub_carrier_spacing_common;
  uint8_t                       ssb_subcarrier_offset = 0;
  dmrs_type_a_position_e_       dmrs_type_a_position;
  pdcch_cfg_sib1_s              pdcch_cfg_sib1;
  cell_barred_e_                cell_barred;
  intra_freq_resel_e_           intra_freq_resel;
  fixed_bitstring<1>            spare;

  static const asn1::table::seq_desc desc;
};

// BCCH-BCH-MessageType ::= CHOICE
struct bcch_bch_msg_type_c {
  struct types_opts {
    enum options { mib, msg_class_ext, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts>;

  void                                  set(types::options e = types::nulltype);
  types                                 type() const { return type_; }
  static const asn1::table::choice_desc desc;
  mib_s&                                mib()
  {
    assert_choice_type(types::mib, type_, "BCCH-BCH-MessageType");
    return c;
  }
  const mib_s& mib() const
  {
    assert_choice_type(types::mib, type_, "BCCH-BCH-MessageType");
    return c;
  }
  mib_s& set_mib();
  void   set_msg_class_ext();

private:
  types type_;
  mib_s c;
};

// BCCH-BCH-Message ::= SEQUENCE
struct bcch_bch_msg_s {
  bcch_bch_msg_type_c msg;

  static const asn1::table::seq_desc desc;
};

} // namespace rrc_nr
} // namespace asn1
