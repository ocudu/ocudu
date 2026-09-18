// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/dl_ssb_pdu.h"

namespace ocudu {
namespace fapi {

/// Helper class to fill in the DL SSB PDU parameters specified in SCF-222 v4.0 section 3.4.2.4.
class dl_ssb_pdu_builder
{
public:
  explicit dl_ssb_pdu_builder(dl_ssb_pdu& pdu_) : pdu(pdu_) {}

  /// \brief Sets the carrier parameters for the fields of the SSB/PBCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.4, in table SSB/PBCH PDU.
  dl_ssb_pdu_builder& set_carrier_parameters(subcarrier_spacing scs)
  {
    pdu.scs = scs;

    return *this;
  }

  /// \brief Sets the cell parameters for the fields of the SSB/PBCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.4, in table SSB/PBCH PDU.
  dl_ssb_pdu_builder& set_cell_parameters(pci_t phys_cell_id)
  {
    pdu.phys_cell_id = phys_cell_id;

    return *this;
  }

  /// \brief Sets the NR power parameters for the fields of the SSB/PBCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.4, in table SSB/PBCH PDU.
  dl_ssb_pdu_builder& set_nr_power_parameters(ssb_pss_to_sss_epre beta_pss)
  {
    auto& power    = pdu.power_config.emplace<dl_ssb_pdu::power_profile_nr>();
    power.beta_pss = beta_pss;

    return *this;
  }

  /// \brief Sets the SSS power parameters for the fields of the SSB/PBCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.4, in table SSB/PBCH PDU.
  dl_ssb_pdu_builder& set_sss_power_parameters(float beta_pss)
  {
    auto& power       = pdu.power_config.emplace<dl_ssb_pdu::power_profile_sss>();
    power.beta_pss_db = beta_pss;

    return *this;
  }

  /// \brief Sets the specific SSB parameters for the fields of the SSB/PBCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.4, in table SSB/PBCH PDU.
  dl_ssb_pdu_builder& set_ssb_parameters(ssb_id_t              ssb_block_index,
                                         ssb_subcarrier_offset subcarrier_offset,
                                         ssb_offset_to_pointA  ssb_offset_pointA,
                                         ssb_pattern_case      case_type,
                                         uint8_t               L_max)
  {
    pdu.ssb_block_index   = ssb_block_index;
    pdu.subcarrier_offset = subcarrier_offset;
    pdu.ssb_offset_pointA = ssb_offset_pointA;
    pdu.case_type         = case_type;
    pdu.L_max             = L_max;

    return *this;
  }

  /// \brief Sets the beamforming parameters for the fields of the SSB/PBCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.5, in table Tx precoding and beamforming PDU.
  dl_ssb_pdu_builder& set_beamforming_parameters(beam_identifier beam_id)
  {
    pdu.beam_id = beam_id;

    return *this;
  }

  /// \brief Sets the BCH payload and returns a reference to the builder. PHY configures the timing PBCH bits.
  ///
  /// Use this function when the PHY generates the timing PBCH information.
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.4, in table MAC generated MIB PDU.
  /// This function assumes that given bch_payload value is codified as: 0,0,0,0,0,0,0,0,a0,a1,a2,...,a21,a22,a23,
  /// with the most significant bit being the leftmost (in this case a0 in position 24 of the uint32_t).
  dl_ssb_pdu_builder& set_bch_payload_phy_timing_info(uint32_t bch_payload)
  {
    static constexpr unsigned MAX_SIZE = (1U << 24U);

    ocudu_assert(bch_payload < MAX_SIZE, "BCH payload value out of bounds");

    // Clear unused bits in the high part of the payload.
    pdu.bch_payload = bch_payload & (MAX_SIZE - 1);

    return *this;
  }

private:
  dl_ssb_pdu& pdu;
};

} // namespace fapi
} // namespace ocudu
