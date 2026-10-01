// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/format.h"
#include "ocudu/fapi/p7/builders/ul_srs_pdu_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(ul_srs_pdu_builder, valid_basic_parameters_passes)
{
  rnti_t rnti = to_rnti(245);

  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  builder.set_ue_specific_parameters(rnti);

  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(0, pdu.handle);
}

TEST(ul_srs_pdu_builder, valid_bwp_parameters_passes)
{
  crb_interval       bwp = {69, 100};
  subcarrier_spacing scs = subcarrier_spacing::kHz30;
  cyclic_prefix      cp  = cyclic_prefix::EXTENDED;

  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  builder.set_bwp_parameters(bwp, scs, cp);

  ASSERT_EQ(bwp, pdu.bwp);
  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cp, pdu.cp);
}

TEST(ul_srs_pdu_builder, valid_timing_parameters_passes)
{
  unsigned        t_offset            = 2;
  srs_periodicity t_srs               = srs_periodicity::sl1;
  unsigned        time_start_position = 3;

  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  builder.set_timing_params(time_start_position, t_srs, t_offset);

  ASSERT_EQ(time_start_position, pdu.time_start_position);
  ASSERT_EQ(t_srs, pdu.t_srs);
  ASSERT_EQ(t_offset, pdu.t_offset);
}

TEST(ul_srs_pdu_builder, valid_comb_parameters_passes)
{
  unsigned     comb_offset = 2;
  tx_comb_size comb_size   = tx_comb_size::n2;

  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  builder.set_comb_params(comb_size, comb_offset);

  ASSERT_EQ(comb_offset, pdu.comb_offset);
  ASSERT_EQ(comb_size, pdu.comb_size);
}

TEST(ul_srs_pdu_builder, valid_frequency_parameters_passes)
{
  unsigned                      frequency_position        = 4;
  unsigned                      frequency_shift           = 3;
  unsigned                      frequency_hopping         = 2;
  srs_group_or_sequence_hopping group_or_sequence_hopping = srs_group_or_sequence_hopping::neither;

  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  builder.set_frequency_params(frequency_position, frequency_shift, frequency_hopping, group_or_sequence_hopping);

  ASSERT_EQ(frequency_position, pdu.frequency_position);
  ASSERT_EQ(frequency_shift, pdu.frequency_shift);
  ASSERT_EQ(frequency_hopping, pdu.frequency_hopping);
  ASSERT_EQ(group_or_sequence_hopping, pdu.group_or_sequence_hopping);
}

TEST(ul_srs_pdu_builder, request_normalized_channel_iq_report_passes)
{
  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  ASSERT_FALSE(pdu.enable_normalized_iq_matrix_report);
  ASSERT_FALSE(pdu.enable_positioning_report);

  builder.set_report_params(true, false);

  ASSERT_TRUE(pdu.enable_normalized_iq_matrix_report);
  ASSERT_FALSE(pdu.enable_positioning_report);

  builder.set_report_params(false, true);

  ASSERT_FALSE(pdu.enable_normalized_iq_matrix_report);
  ASSERT_TRUE(pdu.enable_positioning_report);

  builder.set_report_params(true, true);

  ASSERT_TRUE(pdu.enable_normalized_iq_matrix_report);
  ASSERT_TRUE(pdu.enable_positioning_report);
}

TEST(ul_srs_pdu_builder, valid_srs_parameters_passes)
{
  auto              nof_antenna_ports = srs_resource_configuration::one_two_four_enum::four;
  ofdm_symbol_range ofdm_symbols;
  srs_nof_symbols   nof_repetitions = n1;
  unsigned          config_index    = 2;
  unsigned          sequence_id     = 5;
  unsigned          bandwidth_index = 6;
  unsigned          cyclic_shift    = 7;
  srs_resource_type resource_type   = srs_resource_type::aperiodic;

  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  builder.set_srs_params(nof_antenna_ports,
                         ofdm_symbols,
                         nof_repetitions,
                         config_index,
                         sequence_id,
                         bandwidth_index,
                         cyclic_shift,
                         resource_type);

  ASSERT_EQ(nof_antenna_ports, pdu.num_ant_ports);
  ASSERT_EQ(ofdm_symbols, pdu.ofdm_symbols);
  ASSERT_EQ(nof_repetitions, pdu.num_repetitions);
  ASSERT_EQ(config_index, pdu.config_index);
  ASSERT_EQ(sequence_id, pdu.sequence_id);
  ASSERT_EQ(bandwidth_index, pdu.bandwidth_index);
  ASSERT_EQ(cyclic_shift, pdu.cyclic_shift);
  ASSERT_EQ(resource_type, pdu.resource_type);
}
