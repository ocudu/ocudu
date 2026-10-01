// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/ul_pucch_pdu.h"

namespace ocudu {
namespace fapi {

/// PUCCH format 3 PDU builder that helps to fill in the parameters specified in SCF-222 v4.0 section 3.4.3.3.
class ul_pucch_format_3_pdu_builder
{
  ul_pucch_pdu_format_3& pdu;

public:
  explicit ul_pucch_format_3_pdu_builder(ul_pucch_pdu_format_3& pdu_) : pdu(pdu_) {}

  /// \brief Sets the PUCCH PDU modulation parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_format_3_pdu_builder& set_modulation_parameters(bool pi2_bpsk)
  {
    pdu.pi2_bpsk = pi2_bpsk;

    return *this;
  }

  /// \brief Sets the PUCCH PDU hopping parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_format_3_pdu_builder& set_hopping_parameters(uint16_t nid_pucch_hopping)
  {
    pdu.nid_pucch_hopping = nid_pucch_hopping;

    return *this;
  }

  /// \brief Sets the PUCCH PDU scrambling parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_format_3_pdu_builder& set_scrambling_parameters(uint16_t nid_pucch_scrambling)
  {
    pdu.nid_pucch_scrambling = nid_pucch_scrambling;

    return *this;
  }

  /// \brief Sets the PUCCH PDU DMRS parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_format_3_pdu_builder&
  set_dmrs_parameters(bool add_dmrs_flag, uint16_t nid0_pucch_dmrs_scrambling, uint8_t m0_pucch_dmrs_cyclic_shift)
  {
    pdu.add_dmrs_flag              = add_dmrs_flag;
    pdu.nid0_pucch_dmrs_scrambling = nid0_pucch_dmrs_scrambling;
    pdu.m0_pucch_dmrs_cyclic_shift = m0_pucch_dmrs_cyclic_shift;

    return *this;
  }

  /// \brief Sets the PUCCH PDU payload parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_format_3_pdu_builder&
  set_payload_parameters(sr_nof_bits sr_bit_len, units::bits csi_part1_bit_length, units::bits bit_len_harq)
  {
    pdu.sr_bit_len           = sr_bit_len;
    pdu.csi_part1_bit_length = csi_part1_bit_length;
    pdu.bit_len_harq         = bit_len_harq;

    return *this;
  }
};

} // namespace fapi
} // namespace ocudu
