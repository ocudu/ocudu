// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ssb.h"
#include "ocudu/ran/ssb/pbch_mib_pack.h"

using namespace ocudu;
using namespace fapi_adaptor;

/// \brief Encodes the full BCH payload for the given \c fapi_pdu.
/// \return A packed BCH payload, as per TS38.212 Section 7.1.1.
static uint32_t generate_bch_payload(const dl_ssb_pdu& mac_pdu, uint32_t sfn, bool hrf, subcarrier_spacing scs_common)
{
  // Prepare message parameters.
  pbch_mib_message msg = {.sfn                   = sfn,
                          .hrf                   = hrf,
                          .scs_common            = scs_common,
                          .subcarrier_offset     = mac_pdu.subcarrier_offset,
                          .dmrs_typeA_pos        = mac_pdu.mib_data.dmrs_typeA_pos,
                          .pdcch_config_sib1     = mac_pdu.mib_data.pdcch_config_sib1,
                          .cell_barred           = mac_pdu.mib_data.cell_barred,
                          .intrafreq_reselection = mac_pdu.mib_data.intra_freq_reselection,
                          .ssb_block_index       = mac_pdu.ssb_index};

  // Generate payload.
  return pbch_mib_pack(msg);
}

void ocudu::fapi_adaptor::convert_ssb_mac_to_fapi(fapi::dl_ssb_pdu_builder& builder,
                                                  const dl_ssb_pdu&         mac_pdu,
                                                  slot_point                slot)
{
  builder.set_carrier_parameters(mac_pdu.scs)
      .set_cell_parameters(mac_pdu.pci)
      .set_nr_power_parameters(mac_pdu.pss_to_sss_epre)
      .set_ssb_parameters(mac_pdu.ssb_index,
                          mac_pdu.subcarrier_offset.value(),
                          mac_pdu.offset_to_pointA,
                          mac_pdu.ssb_case,
                          mac_pdu.L_max);

  builder.set_bch_payload_phy_timing_info(generate_bch_payload(mac_pdu, slot.sfn(), slot.is_odd_hrf(), slot.scs()) >>
                                          8);
}
