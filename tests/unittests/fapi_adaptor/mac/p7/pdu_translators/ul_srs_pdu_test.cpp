// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "srs.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

TEST(mac_to_fapi_srs_pdu_test, valid_srs_pdu_should_pass)
{
  const srs_info_helper&   pdu_test = build_valid_srs_pdu();
  const srs_info&          mac_pdu  = pdu_test.pdu;
  fapi::ul_srs_pdu         fapi_pdu;
  fapi::ul_srs_pdu_builder builder(fapi_pdu);

  convert_srs_mac_to_fapi(builder, mac_pdu);

  ASSERT_EQ(mac_pdu.crnti, fapi_pdu.rnti);

  // BWP.
  ASSERT_EQ(mac_pdu.bwp_cfg->cp, fapi_pdu.cp);
  ASSERT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs, fapi_pdu.bwp);

  ASSERT_EQ(mac_pdu.nof_antenna_ports, static_cast<uint8_t>(fapi_pdu.num_ant_ports));
  ASSERT_EQ(mac_pdu.symbols, fapi_pdu.ofdm_symbols);
  ASSERT_EQ(0, fapi_pdu.time_start_position);
  ASSERT_EQ(mac_pdu.config_index, fapi_pdu.config_index);
  ASSERT_EQ(mac_pdu.sequence_id, fapi_pdu.sequence_id);
  ASSERT_EQ(mac_pdu.bw_index, fapi_pdu.bandwidth_index);
  ASSERT_EQ(mac_pdu.tx_comb, fapi_pdu.comb_size);
  ASSERT_EQ(mac_pdu.comb_offset, fapi_pdu.comb_offset);
  ASSERT_EQ(mac_pdu.cyclic_shift, fapi_pdu.cyclic_shift);
  ASSERT_EQ(mac_pdu.freq_position, fapi_pdu.frequency_position);
  ASSERT_EQ(mac_pdu.freq_shift, fapi_pdu.frequency_shift);
  ASSERT_EQ(mac_pdu.freq_hopping, fapi_pdu.frequency_hopping);
  ASSERT_EQ(mac_pdu.group_or_seq_hopping, fapi_pdu.group_or_sequence_hopping);
  ASSERT_EQ(mac_pdu.resource_type, fapi_pdu.resource_type);
  ASSERT_EQ(mac_pdu.t_srs_period, fapi_pdu.t_srs);
  ASSERT_EQ(mac_pdu.t_offset, fapi_pdu.t_offset);
  ASSERT_EQ(mac_pdu.normalized_channel_iq_matrix_requested, fapi_pdu.enable_normalized_iq_matrix_report);
  ASSERT_EQ(mac_pdu.positioning_report_requested, fapi_pdu.enable_positioning_report);
}
