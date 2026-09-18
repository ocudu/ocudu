// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "srs.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace fapi_adaptor;

void ocudu::fapi_adaptor::convert_srs_fapi_to_phy(uplink_pdu_slot_repository::srs_pdu& pdu,
                                                  const fapi::ul_srs_pdu&              fapi_pdu,
                                                  unsigned                             sector_id,
                                                  unsigned                             nof_rx_antennas,
                                                  slot_point                           slot,
                                                  unsigned                             ntn_k_mac_slots)
{
  // Fill main context fields.
  ul_srs_context& context                                  = pdu.context;
  context.slot                                             = slot;
  context.rnti                                             = fapi_pdu.rnti;
  context.is_normalized_channel_iq_matrix_report_requested = fapi_pdu.enable_normalized_iq_matrix_report;
  context.is_positioning_report_requested                  = fapi_pdu.enable_positioning_report;

  // Fill SRS resource configuration.
  pdu.config.context                      = srs_context(sector_id, fapi_pdu.rnti);
  pdu.config.slot                         = slot;
  pdu.config.slot_offset                  = ntn_k_mac_slots;
  pdu.config.resource.nof_antenna_ports   = fapi_pdu.num_ant_ports;
  pdu.config.resource.nof_symbols         = static_cast<srs_nof_symbols>(fapi_pdu.ofdm_symbols.length());
  pdu.config.resource.start_symbol        = fapi_pdu.time_start_position;
  pdu.config.resource.configuration_index = fapi_pdu.config_index;
  pdu.config.resource.sequence_id         = fapi_pdu.sequence_id;
  pdu.config.resource.bandwidth_index     = fapi_pdu.bandwidth_index;
  pdu.config.resource.comb_size           = static_cast<tx_comb_size>(fapi_pdu.comb_size);
  pdu.config.resource.comb_offset         = fapi_pdu.comb_offset;
  pdu.config.resource.cyclic_shift        = fapi_pdu.cyclic_shift;
  pdu.config.resource.freq_position       = fapi_pdu.frequency_position;
  pdu.config.resource.freq_shift          = fapi_pdu.frequency_shift;
  pdu.config.resource.freq_hopping        = fapi_pdu.frequency_hopping;
  pdu.config.resource.hopping = static_cast<srs_group_or_sequence_hopping>(fapi_pdu.group_or_sequence_hopping);
  // SRS frequency hopping not currently supported.

  // GCC gives a false positive of type Werror=stringop-overflow.
  // Fill the antenna port indices starting from 0.
  static_vector<unsigned, srs_constants::max_nof_rx_ports> ports(nof_rx_antennas);
  std::iota(ports.begin(), ports.end(), 0);
  pdu.config.ports.assign(ports.begin(), ports.end());
}
