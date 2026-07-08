// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/to_array.h"
#include "ocudu/fapi/p7/builders/ul_pusch_pdu_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(ul_pusch_pdu_builder, valid_ue_specific_parameters_passes)
{
  rnti_t rnti = to_rnti(245);

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_ue_specific_parameters(rnti);

  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(0, pdu.handle);
}

TEST(ul_pusch_pdu_builder, valid_bwp_parameters_passes)
{
  crb_interval       bwp = {69, 100};
  subcarrier_spacing scs = subcarrier_spacing::kHz30;
  cyclic_prefix      cp  = cyclic_prefix::EXTENDED;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_bwp_parameters(bwp, scs, cp);

  ASSERT_EQ(bwp, pdu.bwp);
  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cp, pdu.cp);
}

TEST(ul_pusch_pdu_builder, valid_information_parameters_passes)
{
  float             target_code_rate = 32.1f;
  modulation_scheme qam_mod_order    = modulation_scheme::QAM256;
  unsigned          mcs_index        = 4;
  pusch_mcs_table   mcs_table        = pusch_mcs_table::qam256;
  unsigned          nid_pusch        = 890;
  unsigned          num_layers       = 2;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_information_parameters(target_code_rate, qam_mod_order, mcs_index, mcs_table, nid_pusch, num_layers);

  ASSERT_EQ(target_code_rate, pdu.target_code_rate);
  ASSERT_EQ(qam_mod_order, pdu.qam_mod_order);
  ASSERT_EQ(mcs_index, pdu.mcs_index);
  ASSERT_EQ(mcs_table, pdu.mcs_table);
  ASSERT_EQ(nid_pusch, pdu.nid_pusch);
  ASSERT_EQ(num_layers, pdu.num_layers);
}

TEST(ul_pusch_pdu_builder, valid_transform_precoding_enabled_parameters_passes)
{
  unsigned pusch_dmrs_identity = 567;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_transform_precoding_enabled_parameters(pusch_dmrs_identity);

  const auto* tp_enabled = std::get_if<ul_pusch_pdu::transform_precoding_enabled>(&pdu.transform_precoding);
  ASSERT_TRUE(tp_enabled);
  ASSERT_EQ(pusch_dmrs_identity, tp_enabled->pusch_dmrs_identity);
}

TEST(ul_pusch_pdu_builder, valid_transform_precoding_disabled_parameters_passes)
{
  unsigned num_dmrs_cdm_grps_no_data = 2;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_transform_precoding_disabled_parameters(num_dmrs_cdm_grps_no_data);

  const auto* tp_disabled = std::get_if<ul_pusch_pdu::transform_precoding_disabled>(&pdu.transform_precoding);
  ASSERT_TRUE(tp_disabled);
  ASSERT_EQ(num_dmrs_cdm_grps_no_data, tp_disabled->num_dmrs_cdm_grps_no_data);
}

TEST(ul_pusch_pdu_builder, valid_dmrs_parameters_passes)
{
  dmrs_symbol_mask ul_dmrs_sym_pos(13);
  ul_dmrs_sym_pos.from_uint64(98);
  dmrs_config_type dmrs_type                = dmrs_config_type::type2;
  unsigned         pusch_dmrs_scrambling_id = 54;
  unsigned         nscid                    = 0;
  dmrs_ports_mask  dmrs_ports(11);
  dmrs_ports.from_uint64(213);

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_dmrs_parameters(ul_dmrs_sym_pos, dmrs_type, pusch_dmrs_scrambling_id, nscid, dmrs_ports);

  ASSERT_EQ(ul_dmrs_sym_pos, pdu.ul_dmrs_symb_pos);
  ASSERT_EQ(pusch_dmrs_scrambling_id, pdu.pusch_dmrs_scrambling_id);
  ASSERT_EQ(nscid, pdu.nscid);
  ASSERT_EQ(dmrs_ports, pdu.dmrs_ports);
}

TEST(ul_pusch_pdu_builder, valid_time_allocation_parameters_passes)
{
  ofdm_symbol_range symbols = {4, 6};

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_time_allocation_parameters(symbols);

  ASSERT_EQ(symbols, pdu.symbols);
}

TEST(ul_pusch_pdu_builder, valid_data_parameters_passes)
{
  unsigned     rv_index        = 2;
  harq_id_t    harq_process_id = to_harq_id(9);
  bool         new_data        = false;
  units::bytes tb_size{89898989};

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.add_optional_pusch_data(rv_index, harq_process_id, new_data, tb_size);

  ASSERT_TRUE(pdu.pusch_data.has_value());
  const auto& data = *pdu.pusch_data;
  ASSERT_EQ(rv_index, data.rv_index);
  ASSERT_EQ(harq_process_id, data.harq_process_id);
  ASSERT_EQ(new_data, data.new_data);
  ASSERT_EQ(tb_size, data.tb_size);
}

TEST(ul_pusch_pdu_builder, valid_uci_parameters_passes)
{
  units::bits                    harq_ack_bit(3);
  units::bits                    csi_part1_bit(5);
  std::vector<alpha_scaling_opt> alpha_scaling_vector = {
      alpha_scaling_opt::f0p5, alpha_scaling_opt::f0p65, alpha_scaling_opt::f0p8, alpha_scaling_opt::f1};
  unsigned beta_offset_harq_ack = 12;
  unsigned beta_offset_csi1     = 2;
  unsigned beta_offset_csi2     = 3;

  for (auto alpha_scaling : alpha_scaling_vector) {
    ul_pusch_pdu         pdu;
    ul_pusch_pdu_builder builder(pdu);

    builder.add_optional_pusch_uci(
        harq_ack_bit, csi_part1_bit, alpha_scaling, beta_offset_harq_ack, beta_offset_csi1, beta_offset_csi2);

    ASSERT_TRUE(pdu.pusch_uci.has_value());
    const auto& uci = *pdu.pusch_uci;
    ASSERT_EQ(harq_ack_bit, uci.harq_ack_bit);
    ASSERT_EQ(csi_part1_bit, uci.csi_part1_bit);
    ASSERT_EQ(alpha_scaling, uci.alpha_scaling);
    ASSERT_EQ(beta_offset_harq_ack, uci.beta_offset_harq_ack);
    ASSERT_EQ(beta_offset_csi1, uci.beta_offset_csi1);
    ASSERT_EQ(beta_offset_csi2, uci.beta_offset_csi2);
  }
}

TEST(ul_pusch_pdu_builder, valid_freq_allocation_parameters_passes)
{
  vrb_interval         vrbs              = {13, 127};
  unsigned             tx_direct_current = 54;
  ldpc_base_graph_type ldcp_graph        = ldpc_base_graph_type::BG2;
  units::bytes         tb_size{3232};

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  builder.set_frequency_allocation_parameters(vrbs, tx_direct_current, ldcp_graph, tb_size);

  ASSERT_EQ(tx_direct_current, pdu.tx_direct_current_location);
  ASSERT_EQ(vrbs, pdu.resource_allocation_1.vrbs);
  ASSERT_EQ(ldcp_graph, pdu.ldpc_base_graph);
  ASSERT_EQ(tb_size, pdu.tb_size_lbrm_bytes);
}

TEST(ul_pusch_pdu_builder, valid_uci_part1_part2_correspondence_parameters_passes)
{
  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  ASSERT_FALSE(pdu.uci_correspondence.has_value());

  unsigned nof_part2 = 2;
  for (unsigned i = 0; i != nof_part2; ++i) {
    static_vector<uint16_t, 4> offset               = {1, 2, 3};
    static_vector<uint8_t, 4>  size                 = {6, 3, 2};
    unsigned                   part2_size_map_index = 31 * (i + 1);

    builder.add_uci_part1_part2_correspondence({offset}, {size}, part2_size_map_index);

    ASSERT_TRUE(pdu.uci_correspondence.has_value());
    ASSERT_EQ(i + 1, pdu.uci_correspondence->part2.size());
    const auto& correspondence = pdu.uci_correspondence->part2.back();
    ASSERT_EQ(part2_size_map_index, correspondence.part2_size_map_index);
    ASSERT_EQ(offset, correspondence.param_offsets);
    ASSERT_EQ(size, correspondence.param_sizes);
  }

  ASSERT_EQ(nof_part2, pdu.uci_correspondence->part2.size());
}

TEST(ul_pusch_pdu_builder, valid_rapid_parameter_passes)
{
  uint8_t rapid = 123;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  ASSERT_FALSE(pdu.rapid.has_value());

  builder.add_optional_rapid(rapid);

  ASSERT_TRUE(pdu.rapid.has_value());
  ASSERT_EQ(pdu.rapid, rapid);
}

TEST(ul_pusch_pdu_builder, valid_tti_bundling_parameters)
{
  uint8_t tb_over_ms_num_total_slots = 11;
  uint8_t tb_over_ms_remaining_slots = 12;

  ul_pusch_pdu         pdu;
  ul_pusch_pdu_builder builder(pdu);

  ASSERT_FALSE(pdu.repetitions.has_value());

  builder.add_tti_bundling_parameters(tb_over_ms_num_total_slots, tb_over_ms_remaining_slots);

  ASSERT_TRUE(pdu.repetitions.has_value());
  ASSERT_EQ(pdu.repetitions->tb_over_ms_num_total_slots, tb_over_ms_num_total_slots);
  ASSERT_EQ(pdu.repetitions->tb_over_ms_remaining_slots, tb_over_ms_remaining_slots);
}
