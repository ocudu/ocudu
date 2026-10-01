// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/format.h"
#include "ocudu/fapi/p7/builders/dl_pdsch_pdu_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(dl_pdsch_pdu_builder, valid_codeword_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  auto builder_cw = builder.add_codeword();

  modulation_scheme qam_mod = modulation_scheme::QAM64;
  sch_mcs_index     mcs_index(2);
  pdsch_mcs_table   mcs_table = pdsch_mcs_table::qam64;
  uint8_t           rv_index  = 2;
  units::bytes      tb_size{128};

  builder_cw.set_codeword_parameters(qam_mod, mcs_index, mcs_table, rv_index, tb_size);

  ASSERT_EQ(mcs_table, pdu.cws[0].mcs_table);
  ASSERT_EQ(tb_size, pdu.cws[0].tb_size);
  ASSERT_EQ(mcs_index, pdu.cws[0].mcs_index);
  ASSERT_EQ(rv_index, pdu.cws[0].rv_index);
  ASSERT_EQ(qam_mod, pdu.cws[0].qam_mod_order);
}

TEST(dl_pdsch_pdu_builder, valid_ue_specific_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  rnti_t rnti = to_rnti(29);
  builder.set_ue_specific_parameters(rnti);

  ASSERT_EQ(rnti, pdu.rnti);
}

TEST(dl_pdsch_pdu_builder, valid_bwp_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  crb_interval       bwp     = {128, 192};
  cyclic_prefix      cprefix = cyclic_prefix::NORMAL;
  subcarrier_spacing scs     = subcarrier_spacing::kHz15;

  builder.set_bwp_parameters(bwp, scs, cprefix);

  ASSERT_EQ(bwp, pdu.bwp);
  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cprefix, pdu.cp);
}

TEST(dl_pdsch_pdu_builder, valid_codewords_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  unsigned             num_layers = 1;
  pdsch_ref_point_type ref_point  = pdsch_ref_point_type::point_a;
  unsigned             n_id_pdsch = 469;
  builder.set_codeword_generation_parameters(n_id_pdsch, num_layers, ref_point);

  ASSERT_EQ(n_id_pdsch, pdu.nid_pdsch);
  ASSERT_EQ(num_layers, pdu.num_layers);
  ASSERT_EQ(ref_point, pdu.ref_point);
}

TEST(dl_pdsch_pdu_builder, add_codeword_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  ASSERT_TRUE(pdu.cws.empty());

  builder.add_codeword();
  ASSERT_EQ(1, pdu.cws.size());
}

TEST(dl_pdsch_pdu_builder, valid_dmrs_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  unsigned         dmrs_scrambling_id = 20;
  dmrs_symbol_mask dmrs_symbol_pos(13);
  dmrs_symbol_pos.from_uint64(3);
  dmrs_ports_mask dmrs_ports(11);
  dmrs_ports.from_uint64(4);
  unsigned         num_dmrs_cdm_grp_no_data = 2;
  unsigned         nscid                    = 1;
  dmrs_config_type config_type              = dmrs_config_type::type1;

  builder.set_dmrs_parameters(
      dmrs_symbol_pos, config_type, dmrs_scrambling_id, nscid, num_dmrs_cdm_grp_no_data, dmrs_ports);

  ASSERT_EQ(dmrs_symbol_pos, pdu.dl_dmrs_symb_pos);
  ASSERT_EQ(config_type, pdu.dmrs_type);
  ASSERT_EQ(dmrs_scrambling_id, pdu.pdsch_dmrs_scrambling_id);
  ASSERT_EQ(nscid, pdu.nscid);
  ASSERT_EQ(num_dmrs_cdm_grp_no_data, pdu.num_dmrs_cdm_grps_no_data);
  ASSERT_EQ(dmrs_ports, pdu.dmrs_ports);
}

TEST(dl_pdsch_pdu_builder, valid_freq_allocation_type_1_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  vrb_to_prb::mapping_type vrb_to_prb = vrb_to_prb::mapping_type::interleaved_n2;
  vrb_interval             vrbs       = {15, 20};

  builder.set_frequency_allocation_type_1(vrbs, vrb_to_prb);

  ASSERT_EQ(vrb_to_prb, pdu.vrb_to_prb_mapping);
  ASSERT_EQ(vrbs, pdu.resource_alloc.vrbs);
}

TEST(dl_pdsch_pdu_builder, valid_time_allocation_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  ofdm_symbol_range symbols = {4, 8};

  builder.set_time_allocation_parameters(symbols);

  ASSERT_EQ(symbols, pdu.symbols);
}

TEST(dl_pdsch_pdu_builder, valid_tx_power_info_parameters_passes)
{
  for (int power : {-8, 16}) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    power_control_offset_ss ss_profile = power_control_offset_ss::dB0;

    builder.set_profile_nr_tx_power_info_parameters(power, ss_profile);

    const auto* profile_nr = std::get_if<dl_pdsch_pdu::power_profile_nr>(&pdu.power_config);
    ASSERT_TRUE(profile_nr != nullptr);
    ASSERT_EQ(power, profile_nr->pwr_control_offset_db);
    ASSERT_EQ(ss_profile, profile_nr->pwr_control_offset_ss);
  }
}

TEST(dl_pdsch_pdu_builder, valid_tx_power_profile_sss_info_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  float dmrs = 22.5;
  float data = -10.99;

  builder.set_profile_sss_tx_power_info_parameters(dmrs, data);

  const auto* profile_sss = std::get_if<dl_pdsch_pdu::power_profile_sss>(&pdu.power_config);
  ASSERT_TRUE(profile_sss != nullptr);
  ASSERT_FLOAT_EQ(dmrs, profile_sss->dmrs_pwr_offset_db);
  ASSERT_FLOAT_EQ(data, profile_sss->data_pwr_offset_db);
}

TEST(dl_pdsch_pdu_builder, valid_set_ldpc_base_graph_passes)
{
  for (auto graph_type : {ldpc_base_graph_type::BG2, ldpc_base_graph_type::BG1}) {
    dl_pdsch_pdu         pdu;
    dl_pdsch_pdu_builder builder(pdu);

    builder.set_ldpc_base_graph(graph_type);

    ASSERT_EQ(graph_type, pdu.ldpc_base_graph);
  }
}

TEST(dl_pdsch_pdu_builder, valid_set_vrb_to_prb_non_interleaved_common_ss_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);
  builder.set_vrb_to_prb_non_interleaved_common_ss_parameters();

  const auto* vrb_to_prb = std::get_if<dl_pdsch_pdu::non_interleaved_common_ss>(&pdu.mapping);
  ASSERT_TRUE(vrb_to_prb);
}

TEST(dl_pdsch_pdu_builder, valid_set_vrb_to_prb_non_interleaved_other_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);
  builder.set_vrb_to_prb_non_interleaved_other_parameters();

  const auto* vrb_to_prb = std::get_if<dl_pdsch_pdu::non_interleaved_other>(&pdu.mapping);

  ASSERT_TRUE(vrb_to_prb);
}

TEST(dl_pdsch_pdu_builder, valid_set_vrb_to_prb_interleaved_common_type0_coreset0_passes)
{
  unsigned N_start_coreset = 10;
  unsigned N_bwp_init_size = 20;

  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);
  builder.set_vrb_to_prb_interleaved_common_type0_coreset0_parameters(N_start_coreset, N_bwp_init_size);

  const auto* vrb_to_prb = std::get_if<dl_pdsch_pdu::interleaved_common_type0_coreset0>(&pdu.mapping);

  ASSERT_TRUE(vrb_to_prb);
  ASSERT_EQ(N_start_coreset, vrb_to_prb->N_start_coreset);
  ASSERT_EQ(N_bwp_init_size, vrb_to_prb->N_bwp_init_size);
}

TEST(dl_pdsch_pdu_builder, valid_set_vrb_to_prb_interleaved_common_any_coreset0_present_passes)
{
  unsigned N_start_coreset = 10;
  unsigned N_bwp_init_size = 20;

  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);
  builder.set_vrb_to_prb_interleaved_common_any_coreset0_present_parameters(N_start_coreset, N_bwp_init_size);

  const auto* vrb_to_prb = std::get_if<dl_pdsch_pdu::interleaved_common_any_coreset0_present>(&pdu.mapping);

  ASSERT_TRUE(vrb_to_prb);
  ASSERT_EQ(N_start_coreset, vrb_to_prb->N_start_coreset);
  ASSERT_EQ(N_bwp_init_size, vrb_to_prb->N_bwp_init_size);
}

TEST(dl_pdsch_pdu_builder, valid_set_vrb_to_prb_interleaved_other_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);
  builder.set_vrb_to_prb_interleaved_other_parameters();

  const auto* vrb_to_prb = std::get_if<dl_pdsch_pdu::interleaved_other>(&pdu.mapping);

  ASSERT_TRUE(vrb_to_prb);
}

TEST(dl_pdsch_pdu_builder, valid_maintenance_v3_csi_rm_parameters_passes)
{
  dl_pdsch_pdu         pdu;
  dl_pdsch_pdu_builder builder(pdu);

  uint16_t nof_csi_pdus = 2;

  builder.set_number_of_csi_puds(nof_csi_pdus);

  ASSERT_EQ(nof_csi_pdus, pdu.nof_csi_pdus_for_rm);
}
