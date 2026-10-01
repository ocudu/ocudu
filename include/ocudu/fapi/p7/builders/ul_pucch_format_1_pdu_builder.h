// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/ul_pucch_pdu.h"

namespace ocudu {
namespace fapi {

/// PUCCH format 1 PDU builder that helps to fill in the parameters specified in SCF-222 v4.0 section 3.4.3.3.
class ul_pucch_format_1_pdu_builder
{
  ul_pucch_pdu_format_1& pdu;

public:
  explicit ul_pucch_format_1_pdu_builder(ul_pucch_pdu_format_1& pdu_) : pdu(pdu_) {}

  /// \brief Sets the PUCCH PDU hopping parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_format_1_pdu_builder& set_hopping_parameters(uint16_t nid_pucch_hopping)
  {
    pdu.nid_pucch_hopping = nid_pucch_hopping;

    return *this;
  }

  /// \brief Sets the PUCCH PDU cylic shift parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_format_1_pdu_builder& set_cyclic_shift_parameters(uint16_t initial_cyclic_shift)
  {
    pdu.initial_cyclic_shift = initial_cyclic_shift;

    return *this;
  }

  /// \brief Sets the PUCCH PDU time domain parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_format_1_pdu_builder& set_time_domain_parameters(uint8_t time_domain_occ_index)
  {
    pdu.time_domain_occ_index = time_domain_occ_index;

    return *this;
  }

  /// \brief Sets the PUCCH PDU payload parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_format_1_pdu_builder& set_payload_parameters(bool sr_present, units::bits bit_len_harq)
  {
    pdu.sr_present   = sr_present;
    pdu.bit_len_harq = bit_len_harq;

    return *this;
  }
};

} // namespace fapi
} // namespace ocudu
