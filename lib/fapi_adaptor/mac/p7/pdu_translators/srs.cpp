// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "srs.h"
#include "ocudu/adt/format.h"
#include "ocudu/scheduler/result/srs_info.h"

using namespace ocudu;
using namespace fapi_adaptor;

void fapi_adaptor::convert_srs_mac_to_fapi(fapi::ul_srs_pdu_builder& builder, const srs_info& mac_pdu)
{
  const bwp_configuration& bwp = *mac_pdu.bwp_cfg;

  builder.set_ue_specific_parameters(mac_pdu.crnti)
      .set_bwp_parameters(bwp.crbs, bwp.scs, bwp.cp)
      .set_frequency_params(
          mac_pdu.freq_position, mac_pdu.freq_shift, mac_pdu.freq_hopping, mac_pdu.group_or_seq_hopping)
      .set_comb_params(mac_pdu.tx_comb, mac_pdu.comb_offset)
      .set_timing_params(mac_pdu.symbols.start(), mac_pdu.t_srs_period, mac_pdu.t_offset)
      .set_srs_params(static_cast<srs_resource_configuration::one_two_four_enum>(mac_pdu.nof_antenna_ports),
                      mac_pdu.symbols,
                      mac_pdu.nof_repetitions,
                      mac_pdu.config_index,
                      mac_pdu.sequence_id,
                      mac_pdu.bw_index,
                      mac_pdu.cyclic_shift,
                      mac_pdu.resource_type)
      .set_report_params(mac_pdu.normalized_channel_iq_matrix_requested, mac_pdu.positioning_report_requested);
}
