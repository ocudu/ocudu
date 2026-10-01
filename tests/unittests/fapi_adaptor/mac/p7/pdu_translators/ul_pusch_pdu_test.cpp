// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "pusch.h"
#include "ocudu/fapi_adaptor/uci_part2_correspondence_generator.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

TEST(mac_to_fapi_pusch_pdu_test, valid_pusch_pdu_should_pass)
{
  const ul_sched_info_test_helper& pdu_test = build_valid_pusch_pdu();
  const ul_sched_info&             mac_pdu  = pdu_test.info;
  fapi::ul_pusch_pdu               fapi_pdu;
  fapi::ul_pusch_pdu_builder       builder(fapi_pdu);
  auto                             uci_part2_tools = generate_uci_part2_correspondence(1);

  convert_pusch_mac_to_fapi(builder, mac_pdu, *std::get<0>(uci_part2_tools));

  // BWP.
  const pusch_information& pusch_cfg = mac_pdu.pusch_cfg;
  ASSERT_EQ(pusch_cfg.bwp_cfg->cp, fapi_pdu.cp);
  ASSERT_EQ(pusch_cfg.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(pusch_cfg.bwp_cfg->crbs, fapi_pdu.bwp);

  // Information parameters.
  ASSERT_EQ(pusch_cfg.mcs_descr.target_code_rate, fapi_pdu.target_code_rate);
  ASSERT_EQ(pusch_cfg.mcs_descr.modulation, fapi_pdu.qam_mod_order);
  ASSERT_EQ(pusch_cfg.mcs_index.value(), fapi_pdu.mcs_index);
  ASSERT_EQ(pusch_cfg.mcs_table, fapi_pdu.mcs_table);
  if (pusch_cfg.transform_precoding) {
    ASSERT_TRUE(std::holds_alternative<fapi::ul_pusch_pdu::transform_precoding_enabled>(fapi_pdu.transform_precoding));
  } else {
    ASSERT_TRUE(std::holds_alternative<fapi::ul_pusch_pdu::transform_precoding_disabled>(fapi_pdu.transform_precoding));
  }
  ASSERT_EQ(pusch_cfg.n_id, fapi_pdu.nid_pusch);
  ASSERT_EQ(pusch_cfg.nof_layers, fapi_pdu.num_layers);

  // DMRS.
  const dmrs_information& dmrs_cfg = pusch_cfg.dmrs;
  ASSERT_EQ(dmrs_cfg.dmrs_symb_pos, fapi_pdu.ul_dmrs_symb_pos);
  ASSERT_EQ(dmrs_cfg.config_type, fapi_pdu.dmrs_type);
  ASSERT_EQ(dmrs_cfg.n_scid, fapi_pdu.nscid);

  if (auto const* tp_disabled =
          std::get_if<fapi::ul_pusch_pdu::transform_precoding_disabled>(&fapi_pdu.transform_precoding)) {
    ASSERT_EQ(dmrs_cfg.num_dmrs_cdm_grps_no_data, tp_disabled->num_dmrs_cdm_grps_no_data);
  } else if (auto const* tp_enabled =
                 std::get_if<fapi::ul_pusch_pdu::transform_precoding_enabled>(&fapi_pdu.transform_precoding)) {
    ASSERT_EQ(dmrs_cfg.dmrs_scrambling_id, tp_enabled->pusch_dmrs_identity);
  }

  ASSERT_EQ(dmrs_cfg.dmrs_ports, fapi_pdu.dmrs_ports);

  // Frequency allocation.
  const vrb_alloc& prb_cfg = pusch_cfg.rbs;
  ASSERT_EQ(prb_cfg.type1(), fapi_pdu.resource_allocation_1.vrbs);
  ASSERT_EQ(pusch_cfg.tx_direct_current_location, fapi_pdu.tx_direct_current_location);

  // Time allocation.
  ASSERT_EQ(pusch_cfg.symbols, fapi_pdu.symbols);
  // Maintenance v3.
  ASSERT_EQ(units::bytes(159749), fapi_pdu.tb_size_lbrm_bytes);
  ASSERT_EQ(get_ldpc_base_graph(pusch_cfg.mcs_descr.get_normalised_target_code_rate(),
                                units::bytes(pusch_cfg.tb_size_bytes).to_bits()),
            fapi_pdu.ldpc_base_graph);

  ASSERT_TRUE(fapi_pdu.pusch_data.has_value());
  const fapi::ul_pusch_data& data = *fapi_pdu.pusch_data;
  ASSERT_EQ(pusch_cfg.rv_index, data.rv_index);
  ASSERT_EQ(pusch_cfg.harq_id, data.harq_process_id);
  ASSERT_EQ(pusch_cfg.new_data, data.new_data);
  ASSERT_EQ(pusch_cfg.tb_size_bytes, data.tb_size);

  ASSERT_TRUE(fapi_pdu.pusch_uci.has_value());
  const fapi::ul_pusch_uci& fapi_uci = *fapi_pdu.pusch_uci;
  const uci_info&           mac_uci  = mac_pdu.uci.value();
  ASSERT_EQ(mac_uci.harq.has_value() ? mac_uci.harq->harq_ack_nof_bits : 0U, fapi_uci.harq_ack_bit.value());
  ASSERT_EQ(mac_uci.csi.has_value() ? mac_uci.csi->csi_part1_nof_bits : 0U, fapi_uci.csi_part1_bit.value());
  ASSERT_EQ(mac_uci.alpha, fapi_uci.alpha_scaling);
  ASSERT_EQ(mac_uci.harq.has_value() ? mac_uci.harq->beta_offset_harq_ack : 0U, fapi_uci.beta_offset_harq_ack);
  ASSERT_EQ(mac_uci.csi.has_value() ? mac_uci.csi->beta_offset_csi_1 : 0U, fapi_uci.beta_offset_csi1);
  ASSERT_EQ(mac_uci.csi.has_value() && mac_uci.csi->beta_offset_csi_2.has_value()
                ? mac_uci.csi->beta_offset_csi_2.value()
                : 0U,
            fapi_uci.beta_offset_csi2);

  // Rapid.
  ASSERT_EQ(fapi_pdu.rapid, pdu_test.info.context.rapid);
}
