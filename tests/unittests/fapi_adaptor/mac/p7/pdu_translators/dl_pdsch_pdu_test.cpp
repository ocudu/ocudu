// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "pdsch.h"
#include "ocudu/fapi_adaptor/precoding_matrix_table_generator.h"
#include "ocudu/mac/mac_cell_result.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

static void validate_pdsch_information(const pdsch_information& pdsch_cfg, const fapi::dl_pdsch_pdu& fapi_pdu)
{
  // BWP params
  const bwp_configuration& bwp_cfg = *pdsch_cfg.bwp_cfg;
  ASSERT_EQ(pdsch_cfg.coreset_cfg->coreset0_crbs(), fapi_pdu.bwp);
  ASSERT_EQ(bwp_cfg.scs, fapi_pdu.scs);
  ASSERT_EQ(bwp_cfg.cp, fapi_pdu.cp);

  // Codewords.
  const auto& fapi_cw = fapi_pdu.cws.front();
  const auto& mac_cw  = pdsch_cfg.codewords.front();
  ASSERT_EQ(mac_cw.rv_index, fapi_cw.rv_index);
  ASSERT_EQ(mac_cw.mcs_table, fapi_cw.mcs_table);
  ASSERT_EQ(mac_cw.mcs_index, fapi_cw.mcs_index);
  ASSERT_EQ(mac_cw.mcs_descr.modulation, fapi_cw.qam_mod_order);
  ASSERT_EQ(mac_cw.tb_size_bytes, fapi_cw.tb_size);

  // DMRS.
  const dmrs_information& dmrs_cfg = pdsch_cfg.dmrs;
  ASSERT_EQ(dmrs_cfg.dmrs_symb_pos, fapi_pdu.dl_dmrs_symb_pos);
  ASSERT_EQ(dmrs_cfg.config_type, fapi_pdu.dmrs_type);
  ASSERT_EQ(dmrs_cfg.dmrs_scrambling_id, fapi_pdu.pdsch_dmrs_scrambling_id);
  ASSERT_EQ(dmrs_cfg.n_scid, fapi_pdu.nscid);
  ASSERT_EQ(dmrs_cfg.num_dmrs_cdm_grps_no_data, fapi_pdu.num_dmrs_cdm_grps_no_data);
  ASSERT_EQ(dmrs_cfg.dmrs_ports, fapi_pdu.dmrs_ports);

  // Frequency allocation.
  const vrb_alloc& prb_cfg = pdsch_cfg.rbs;
  ASSERT_EQ(fapi_pdu.vrb_to_prb_mapping, vrb_to_prb::mapping_type::non_interleaved);
  ASSERT_EQ(prb_cfg.type1(), fapi_pdu.resource_alloc.vrbs);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, valid_sib1_pdu_should_pass)
{
  sib_information_test_helper pdu_test     = build_valid_sib1_information_pdu();
  sib_information             pdu          = pdu_test.pdu;
  unsigned                    nof_csi_pdus = 2;
  unsigned                    nof_prbs     = 51U;

  fapi::dl_pdsch_pdu         fapi_pdu;
  fapi::dl_pdsch_pdu_builder builder(fapi_pdu);
  auto                       pm_tools = generate_precoding_matrix_tables(pmi_codebook_one_port{}, 0);
  convert_pdsch_mac_to_fapi(builder, pdu, nof_csi_pdus, *std::get<0>(pm_tools), nof_prbs);

  validate_pdsch_information(pdu.pdsch_cfg, fapi_pdu);

  ASSERT_EQ(nof_csi_pdus, fapi_pdu.nof_csi_pdus_for_rm);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, valid_rar_pdu_should_pass)
{
  rar_information_test_helper pdu_test     = build_valid_rar_information_pdu();
  rar_information             pdu          = pdu_test.pdu;
  unsigned                    nof_csi_pdus = 2;
  unsigned                    nof_prbs     = 51U;

  fapi::dl_pdsch_pdu         fapi_pdu;
  fapi::dl_pdsch_pdu_builder builder(fapi_pdu);
  auto                       pm_tools = generate_precoding_matrix_tables(pmi_codebook_two_port{}, 0);
  convert_pdsch_mac_to_fapi(builder, pdu, nof_csi_pdus, *std::get<0>(pm_tools), nof_prbs);

  validate_pdsch_information(pdu.pdsch_cfg, fapi_pdu);

  ASSERT_EQ(nof_csi_pdus, fapi_pdu.nof_csi_pdus_for_rm);

  const auto& fapi_prec = fapi_pdu.precoding_and_beamforming;
  ASSERT_EQ(nof_prbs, fapi_prec.prg_size);
  ASSERT_FALSE(std::get<1>(pm_tools)->get_precoding_matrix(fapi_prec.prg.pm_index).get_nof_layers() == 0);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, valid_dl_paging_pdu_should_pass)
{
  dl_paging_allocation_test_helper pdu_test     = build_valid_dl_paging_pdu();
  dl_paging_allocation             pdu          = pdu_test.pdu;
  unsigned                         nof_csi_pdus = 2;
  unsigned                         nof_prbs     = 51U;

  fapi::dl_pdsch_pdu         fapi_pdu;
  fapi::dl_pdsch_pdu_builder builder(fapi_pdu);
  auto                       pm_tools = generate_precoding_matrix_tables(pmi_codebook_one_port{}, 0);
  convert_pdsch_mac_to_fapi(builder, pdu, nof_csi_pdus, *std::get<0>(pm_tools), nof_prbs);

  validate_pdsch_information(pdu.pdsch_cfg, fapi_pdu);

  ASSERT_EQ(nof_csi_pdus, fapi_pdu.nof_csi_pdus_for_rm);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, valid_dl_msg_alloc_pdu_should_pass)
{
  dl_msg_allocation_test_helper pdu_test     = build_valid_dl_msg_alloc_pdu();
  dl_msg_alloc                  pdu          = pdu_test.pdu;
  unsigned                      nof_csi_pdus = 2;
  unsigned                      nof_prbs     = 51U;

  fapi::dl_pdsch_pdu         fapi_pdu;
  fapi::dl_pdsch_pdu_builder builder(fapi_pdu);
  auto                       pm_tools = generate_precoding_matrix_tables(
      pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_one, pmi_codebook_typeI_mode::one}, 0);
  convert_pdsch_mac_to_fapi(builder, pdu, nof_csi_pdus, *std::get<0>(pm_tools), nof_prbs);

  validate_pdsch_information(pdu.pdsch_cfg, fapi_pdu);

  ASSERT_EQ(nof_csi_pdus, fapi_pdu.nof_csi_pdus_for_rm);

  // Context.
  ASSERT_EQ(pdu.pdsch_cfg.harq_id, fapi_pdu.context->get_h_id());
  ASSERT_EQ(bool(pdu.context.nof_retxs), !fapi_pdu.context->is_new_data());

  const auto& mac_prec  = *pdu.pdsch_cfg.precoding;
  const auto& fapi_prec = fapi_pdu.precoding_and_beamforming;
  ASSERT_EQ(mac_prec.nof_rbs_per_prg, fapi_prec.prg_size);
  ASSERT_FALSE(std::get<1>(pm_tools)->get_precoding_matrix(fapi_prec.prg.pm_index).get_nof_layers() == 0);
}
