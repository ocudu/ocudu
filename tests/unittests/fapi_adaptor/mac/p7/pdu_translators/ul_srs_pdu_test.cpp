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

/// Converts the given MAC SRS PDU into a FAPI one.
static fapi::ul_srs_pdu convert_srs(const srs_info& mac_pdu)
{
  fapi::ul_srs_pdu         fapi_pdu;
  fapi::ul_srs_pdu_builder builder(fapi_pdu);
  convert_srs_mac_to_fapi(builder, mac_pdu);
  return fapi_pdu;
}

TEST(mac_to_fapi_srs_pdu_test, valid_srs_pdu_should_pass)
{
  srs_info_helper pdu_test = build_valid_srs_pdu();
  const srs_info& mac_pdu  = pdu_test.pdu;

  fapi::ul_srs_pdu fapi_pdu = convert_srs(mac_pdu);

  EXPECT_EQ(mac_pdu.crnti, fapi_pdu.rnti);

  // BWP.
  EXPECT_EQ(mac_pdu.bwp_cfg->cp, fapi_pdu.cp);
  EXPECT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  EXPECT_EQ(mac_pdu.bwp_cfg->crbs, fapi_pdu.bwp);

  EXPECT_EQ(mac_pdu.nof_antenna_ports, static_cast<uint8_t>(fapi_pdu.num_ant_ports));
  EXPECT_EQ(mac_pdu.symbols, fapi_pdu.ofdm_symbols);
  EXPECT_EQ(mac_pdu.nof_repetitions, fapi_pdu.num_repetitions);
  EXPECT_EQ(mac_pdu.symbols.start(), fapi_pdu.time_start_position);
  EXPECT_EQ(mac_pdu.config_index, fapi_pdu.config_index);
  EXPECT_EQ(mac_pdu.sequence_id, fapi_pdu.sequence_id);
  EXPECT_EQ(mac_pdu.bw_index, fapi_pdu.bandwidth_index);
  EXPECT_EQ(mac_pdu.tx_comb, fapi_pdu.comb_size);
  EXPECT_EQ(mac_pdu.comb_offset, fapi_pdu.comb_offset);
  EXPECT_EQ(mac_pdu.cyclic_shift, fapi_pdu.cyclic_shift);
  EXPECT_EQ(mac_pdu.freq_position, fapi_pdu.frequency_position);
  EXPECT_EQ(mac_pdu.freq_shift, fapi_pdu.frequency_shift);
  EXPECT_EQ(mac_pdu.freq_hopping, fapi_pdu.frequency_hopping);
  EXPECT_EQ(mac_pdu.group_or_seq_hopping, fapi_pdu.group_or_sequence_hopping);
  EXPECT_EQ(mac_pdu.resource_type, fapi_pdu.resource_type);
  EXPECT_EQ(mac_pdu.t_srs_period, fapi_pdu.t_srs);
  EXPECT_EQ(mac_pdu.t_offset, fapi_pdu.t_offset);
  EXPECT_EQ(mac_pdu.normalized_channel_iq_matrix_requested, fapi_pdu.enable_normalized_iq_matrix_report);
  EXPECT_EQ(mac_pdu.positioning_report_requested, fapi_pdu.enable_positioning_report);
}

TEST(mac_to_fapi_srs_pdu_test, time_start_position_is_the_first_srs_symbol)
{
  srs_info_helper pdu_test = build_valid_srs_pdu();
  pdu_test.pdu.symbols     = ofdm_symbol_range{10, 12};

  fapi::ul_srs_pdu fapi_pdu = convert_srs(pdu_test.pdu);

  // FAPI carries the starting symbol l0, so that the MAC translates the RRC start position into it.
  EXPECT_EQ(10U, fapi_pdu.time_start_position);
  EXPECT_EQ(pdu_test.pdu.symbols, fapi_pdu.ofdm_symbols);
}

namespace {

class mac_to_fapi_srs_repetition_fixture : public ::testing::TestWithParam<srs_nof_symbols>
{};

} // namespace

TEST_P(mac_to_fapi_srs_repetition_fixture, repetition_factor_is_forwarded)
{
  srs_info_helper pdu_test     = build_valid_srs_pdu();
  pdu_test.pdu.nof_repetitions = GetParam();

  fapi::ul_srs_pdu fapi_pdu = convert_srs(pdu_test.pdu);

  EXPECT_EQ(GetParam(), fapi_pdu.num_repetitions);
}

INSTANTIATE_TEST_SUITE_P(repetition_factors,
                         mac_to_fapi_srs_repetition_fixture,
                         ::testing::Values(srs_nof_symbols::n1, srs_nof_symbols::n2, srs_nof_symbols::n4),
                         [](const ::testing::TestParamInfo<srs_nof_symbols>& param_info) {
                           return fmt::format("repetitions_{}", static_cast<unsigned>(param_info.param));
                         });
