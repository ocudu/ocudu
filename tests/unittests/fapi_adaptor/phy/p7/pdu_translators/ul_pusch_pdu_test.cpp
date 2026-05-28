// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../message_builder_helpers.h"
#include "pusch.h"
#include "ocudu/fapi_adaptor/uci_part2_correspondence_generator.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittest;

// Random generator.
static std::mt19937 rgen;

TEST(fapi_phy_ul_pusch_adaptor_test, valid_pdu_pass)
{
  fapi::ul_pusch_pdu fapi_pdu = build_valid_ul_pusch_pdu();

  auto     scs        = subcarrier_spacing::kHz30;
  unsigned sfn        = 1U;
  unsigned slot_index = 2U;
  auto     slot       = slot_point(scs, sfn, slot_index);

  std::uniform_int_distribution<unsigned> nof_antenna_ports_dist(1, 4);
  unsigned                                nof_antenna_ports = nof_antenna_ports_dist(rgen);
  auto                                    uci_part2_tools   = fapi_adaptor::generate_uci_part2_correspondence(1);

  uplink_pdu_slot_repository::pusch_pdu pdu;
  convert_pusch_fapi_to_phy(pdu,
                            fapi_pdu,
                            slot,
                            nof_antenna_ports,
                            *std::get<std::unique_ptr<uci_part2_correspondence_repository>>(uci_part2_tools));

  const pusch_processor::pdu_t& phy_pdu = pdu.pdu;
  ASSERT_EQ(slot, phy_pdu.slot);
  ASSERT_EQ(fapi_pdu.symbols.start(), phy_pdu.start_symbol_index);
  ASSERT_EQ(fapi_pdu.symbols.length(), phy_pdu.nof_symbols);
  ASSERT_EQ(to_value(fapi_pdu.rnti), phy_pdu.rnti);
  ASSERT_EQ(fapi_pdu.bwp.start(), phy_pdu.bwp_start_rb);
  ASSERT_EQ(fapi_pdu.bwp.length(), phy_pdu.bwp_size_rb);
  ASSERT_EQ(fapi_pdu.cp, phy_pdu.cp);
  ASSERT_EQ(fapi_pdu.qam_mod_order, phy_pdu.mcs_descr.modulation);
  ASSERT_EQ(fapi_pdu.nid_pusch, phy_pdu.n_id);
  ASSERT_EQ(fapi_pdu.num_layers, phy_pdu.nof_tx_layers);
  ASSERT_EQ(nof_antenna_ports, phy_pdu.rx_ports.size());

  if (fapi_pdu.tx_direct_current_location < 3300) {
    ASSERT_TRUE(phy_pdu.dc_position.has_value());
    ASSERT_EQ(fapi_pdu.tx_direct_current_location, phy_pdu.dc_position.value());
  } else {
    ASSERT_FALSE(phy_pdu.dc_position.has_value());
  }

  // DM-RS.
  ASSERT_EQ(fapi_pdu.ul_dmrs_symb_pos, phy_pdu.dmrs_symbol_mask);
  if (const auto* tp_enabled =
          std::get_if<fapi::ul_pusch_pdu::transform_precoding_enabled>(&fapi_pdu.transform_precoding)) {
    const auto& dmrs_config = std::get<pusch_processor::dmrs_transform_precoding_configuration>(phy_pdu.dmrs);
    ASSERT_EQ(tp_enabled->pusch_dmrs_identity, dmrs_config.n_rs_id);
  } else if (const auto* tp_disabled =
                 std::get_if<fapi::ul_pusch_pdu::transform_precoding_disabled>(&fapi_pdu.transform_precoding)) {
    const auto& dmrs_config = std::get<pusch_processor::dmrs_configuration>(phy_pdu.dmrs);
    ASSERT_EQ(fapi_pdu.dmrs_type, dmrs_config.dmrs);
    ASSERT_EQ(fapi_pdu.pusch_dmrs_scrambling_id, dmrs_config.scrambling_id);
    ASSERT_EQ(fapi_pdu.nscid, dmrs_config.n_scid);
    ASSERT_EQ(tp_disabled->num_dmrs_cdm_grps_no_data, dmrs_config.nof_cdm_groups_without_data);
  }
  ASSERT_EQ(fapi_pdu.tb_size_lbrm_bytes, phy_pdu.tbs_lbrm);

  // RB allocation.
  rb_allocation alloc = rb_allocation::make_type1(
      fapi_pdu.resource_allocation_1.vrbs.start(), fapi_pdu.resource_allocation_1.vrbs.length(), {});

  ASSERT_EQ(alloc, phy_pdu.freq_alloc);

  // Codeword.
  ASSERT_TRUE(phy_pdu.codeword.has_value());
  ASSERT_EQ(fapi_pdu.pusch_data->rv_index, phy_pdu.codeword.value().rv);
  ASSERT_EQ(fapi_pdu.pusch_data->new_data, phy_pdu.codeword.value().new_data);
  ASSERT_EQ(fapi_pdu.ldpc_base_graph, phy_pdu.codeword.value().ldpc_base_graph);
  ASSERT_EQ(fapi_pdu.pusch_data->tb_size.value(), pdu.tb_size.value());
  ASSERT_EQ(fapi_pdu.pusch_data->harq_process_id, pdu.harq_id);
}
