// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "pusch.h"
#include "ocudu/fapi_adaptor/uci_part2_correspondence_generator.h"
#include "ocudu/ran/sch/sch_constants.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

namespace {

class mac_to_fapi_pusch_fixture : public ::testing::Test
{
  using uci_part2_tools =
      std::pair<std::unique_ptr<uci_part2_correspondence_mapper>, std::unique_ptr<uci_part2_correspondence_repository>>;

protected:
  /// Converts the current PUSCH PDU and returns the resulting FAPI PDU.
  fapi::ul_pusch_pdu convert()
  {
    fapi::ul_pusch_pdu         fapi_pdu;
    fapi::ul_pusch_pdu_builder builder(fapi_pdu);
    convert_pusch_mac_to_fapi(builder, pdu_test.info, *uci_tools.first);
    return fapi_pdu;
  }

  ul_sched_info_test_helper pdu_test  = build_valid_pusch_pdu();
  uci_part2_tools           uci_tools = generate_uci_part2_correspondence(1);
};

} // namespace

TEST_F(mac_to_fapi_pusch_fixture, valid_pusch_pdu_should_pass)
{
  fapi::ul_pusch_pdu fapi_pdu = convert();

  // BWP.
  const pusch_information& pusch_cfg = pdu_test.info.pusch_cfg;
  EXPECT_EQ(pusch_cfg.bwp_cfg->cp, fapi_pdu.cp);
  EXPECT_EQ(pusch_cfg.bwp_cfg->scs, fapi_pdu.scs);
  EXPECT_EQ(pusch_cfg.bwp_cfg->crbs, fapi_pdu.bwp);

  // Information parameters.
  EXPECT_EQ(pusch_cfg.mcs_descr.target_code_rate, fapi_pdu.target_code_rate);
  EXPECT_EQ(pusch_cfg.mcs_descr.modulation, fapi_pdu.qam_mod_order);
  EXPECT_EQ(pusch_cfg.mcs_index.value(), fapi_pdu.mcs_index);
  EXPECT_EQ(pusch_cfg.mcs_table, fapi_pdu.mcs_table);
  EXPECT_EQ(pusch_cfg.n_id, fapi_pdu.nid_pusch);
  EXPECT_EQ(pusch_cfg.nof_layers, fapi_pdu.num_layers);

  // DMRS. The helper disables transform precoding.
  const dmrs_information& dmrs_cfg = pusch_cfg.dmrs;
  EXPECT_EQ(dmrs_cfg.dmrs_symb_pos, fapi_pdu.ul_dmrs_symb_pos);
  EXPECT_EQ(dmrs_cfg.config_type, fapi_pdu.dmrs_type);
  EXPECT_EQ(dmrs_cfg.n_scid, fapi_pdu.nscid);
  EXPECT_EQ(dmrs_cfg.dmrs_scrambling_id, fapi_pdu.pusch_dmrs_scrambling_id);
  EXPECT_EQ(dmrs_cfg.dmrs_ports, fapi_pdu.dmrs_ports);
  const auto* tp_disabled =
      std::get_if<fapi::ul_pusch_pdu::transform_precoding_disabled>(&fapi_pdu.transform_precoding);
  ASSERT_NE(tp_disabled, nullptr);
  EXPECT_EQ(dmrs_cfg.num_dmrs_cdm_grps_no_data, tp_disabled->num_dmrs_cdm_grps_no_data);

  // Frequency and time allocation.
  EXPECT_EQ(pusch_cfg.rbs.type1(), fapi_pdu.resource_allocation_1.vrbs);
  EXPECT_EQ(pusch_cfg.tx_direct_current_location, fapi_pdu.tx_direct_current_location);
  EXPECT_EQ(pusch_cfg.symbols, fapi_pdu.symbols);

  // Maintenance v3.
  EXPECT_EQ(tbs_lbrm_default, fapi_pdu.tb_size_lbrm_bytes);
  EXPECT_EQ(get_ldpc_base_graph(pusch_cfg.mcs_descr.get_normalised_target_code_rate(),
                                units::bytes(pusch_cfg.tb_size_bytes).to_bits()),
            fapi_pdu.ldpc_base_graph);

  // Data.
  ASSERT_TRUE(fapi_pdu.pusch_data.has_value());
  const fapi::ul_pusch_data& data = *fapi_pdu.pusch_data;
  EXPECT_EQ(pusch_cfg.rv_index, data.rv_index);
  EXPECT_EQ(pusch_cfg.harq_id, data.harq_process_id);
  EXPECT_EQ(pusch_cfg.new_data, data.new_data);
  EXPECT_EQ(pusch_cfg.harq_id, fapi_pdu.harq_id);
  EXPECT_EQ(pusch_cfg.tb_size_bytes, data.tb_size);

  // UCI. The helper requests HARQ-ACK and CSI Part 1 and 2.
  ASSERT_TRUE(fapi_pdu.pusch_uci.has_value());
  const fapi::ul_pusch_uci& fapi_uci = *fapi_pdu.pusch_uci;
  const uci_info&           mac_uci  = *pdu_test.info.uci;
  EXPECT_EQ(mac_uci.alpha, fapi_uci.alpha_scaling);
  EXPECT_EQ(mac_uci.harq->harq_ack_nof_bits, fapi_uci.harq_ack_bit.value());
  EXPECT_EQ(mac_uci.harq->beta_offset_harq_ack, fapi_uci.beta_offset_harq_ack);
  EXPECT_EQ(mac_uci.csi->csi_part1_nof_bits, fapi_uci.csi_part1_bit.value());
  EXPECT_EQ(mac_uci.csi->beta_offset_csi_1, fapi_uci.beta_offset_csi1);
  EXPECT_EQ(*mac_uci.csi->beta_offset_csi_2, fapi_uci.beta_offset_csi2);

  EXPECT_EQ(pdu_test.info.context.rapid, fapi_pdu.rapid);
}

TEST_F(mac_to_fapi_pusch_fixture, transform_precoding_enabled_carries_the_pusch_dmrs_identity)
{
  pusch_information& pusch = pdu_test.info.pusch_cfg;
  // The two identifiers must differ, otherwise the test cannot tell which one is forwarded.
  ASSERT_NE(pusch.dmrs.dmrs_scrambling_id, pusch.pusch_dmrs_id);
  pusch.transform_precoding = true;

  fapi::ul_pusch_pdu fapi_pdu = convert();

  const auto* enabled = std::get_if<fapi::ul_pusch_pdu::transform_precoding_enabled>(&fapi_pdu.transform_precoding);
  ASSERT_NE(enabled, nullptr);
  EXPECT_EQ(pusch.pusch_dmrs_id, enabled->pusch_dmrs_identity);
  // The DMRS scrambling identifier does not depend on the transform precoding.
  EXPECT_EQ(pusch.dmrs.dmrs_scrambling_id, fapi_pdu.pusch_dmrs_scrambling_id);
}

TEST_F(mac_to_fapi_pusch_fixture, dmrs_scrambling_id_comes_from_the_dmrs_configuration)
{
  pusch_information& pusch = pdu_test.info.pusch_cfg;
  // The two identifiers must differ, otherwise the test cannot tell which one is forwarded.
  ASSERT_NE(pusch.dmrs.dmrs_scrambling_id, pusch.pusch_dmrs_id);
  pusch.transform_precoding = false;

  fapi::ul_pusch_pdu fapi_pdu = convert();

  EXPECT_EQ(pusch.dmrs.dmrs_scrambling_id, fapi_pdu.pusch_dmrs_scrambling_id);
}

TEST_F(mac_to_fapi_pusch_fixture, pdu_without_uci_has_no_uci_information)
{
  pdu_test.info.uci.reset();

  fapi::ul_pusch_pdu fapi_pdu = convert();

  EXPECT_FALSE(fapi_pdu.pusch_uci.has_value());
  EXPECT_FALSE(fapi_pdu.uci_correspondence.has_value());
}

TEST_F(mac_to_fapi_pusch_fixture, harq_only_uci_zeroes_the_csi_fields)
{
  pdu_test.info.uci->csi.reset();

  fapi::ul_pusch_pdu fapi_pdu = convert();

  ASSERT_TRUE(fapi_pdu.pusch_uci.has_value());
  const fapi::ul_pusch_uci& uci = *fapi_pdu.pusch_uci;
  EXPECT_EQ(pdu_test.info.uci->harq->harq_ack_nof_bits, uci.harq_ack_bit.value());
  EXPECT_EQ(pdu_test.info.uci->harq->beta_offset_harq_ack, uci.beta_offset_harq_ack);
  EXPECT_EQ(0U, uci.csi_part1_bit.value());
  EXPECT_EQ(0U, uci.beta_offset_csi1);
  EXPECT_EQ(0U, uci.beta_offset_csi2);
  EXPECT_FALSE(fapi_pdu.uci_correspondence.has_value());
}

TEST_F(mac_to_fapi_pusch_fixture, csi_only_uci_zeroes_the_harq_fields)
{
  pdu_test.info.uci->harq.reset();

  fapi::ul_pusch_pdu fapi_pdu = convert();

  ASSERT_TRUE(fapi_pdu.pusch_uci.has_value());
  const fapi::ul_pusch_uci& uci = *fapi_pdu.pusch_uci;
  EXPECT_EQ(0U, uci.harq_ack_bit.value());
  EXPECT_EQ(0U, uci.beta_offset_harq_ack);
  EXPECT_EQ(pdu_test.info.uci->csi->csi_part1_nof_bits, uci.csi_part1_bit.value());
  EXPECT_EQ(pdu_test.info.uci->csi->beta_offset_csi_1, uci.beta_offset_csi1);
}

TEST_F(mac_to_fapi_pusch_fixture, csi_part2_correspondence_is_forwarded)
{
  const uci_info::csi_info& csi = *pdu_test.info.uci->csi;
  // The mapper is the source of the expected correspondence. Copy it, as it is only valid until the next call.
  span<const uci_part2_correspondence_information>  mapped = uci_tools.first->map(csi.csi_rep_cfg);
  std::vector<uci_part2_correspondence_information> expected(mapped.begin(), mapped.end());
  ASSERT_FALSE(expected.empty()) << "The test configuration must produce a CSI Part 2 correspondence";

  fapi::ul_pusch_pdu fapi_pdu = convert();

  ASSERT_TRUE(fapi_pdu.uci_correspondence.has_value());
  const auto& part2_entries = fapi_pdu.uci_correspondence->part2;
  ASSERT_EQ(expected.size(), part2_entries.size());
  for (unsigned i = 0, e = expected.size(); i != e; ++i) {
    SCOPED_TRACE(i);
    EXPECT_EQ(std::vector<uint16_t>(expected[i].part1_param_offsets.begin(), expected[i].part1_param_offsets.end()),
              std::vector<uint16_t>(part2_entries[i].param_offsets.begin(), part2_entries[i].param_offsets.end()));
    EXPECT_EQ(std::vector<uint8_t>(expected[i].part1_param_sizes.begin(), expected[i].part1_param_sizes.end()),
              std::vector<uint8_t>(part2_entries[i].param_sizes.begin(), part2_entries[i].param_sizes.end()));
    EXPECT_EQ(expected[i].part2_map_index, part2_entries[i].part2_size_map_index);
  }

  ASSERT_TRUE(fapi_pdu.pusch_uci.has_value());
  EXPECT_EQ(*csi.beta_offset_csi_2, fapi_pdu.pusch_uci->beta_offset_csi2);
}

TEST_F(mac_to_fapi_pusch_fixture, csi_part2_is_skipped_for_a_single_antenna_port)
{
  uci_info::csi_info& csi = *pdu_test.info.uci->csi;
  ASSERT_TRUE(csi.beta_offset_csi_2.has_value());
  csi.csi_rep_cfg.pmi_codebook = pmi_codebook_one_port{};

  fapi::ul_pusch_pdu fapi_pdu = convert();

  EXPECT_FALSE(fapi_pdu.uci_correspondence.has_value());
  ASSERT_TRUE(fapi_pdu.pusch_uci.has_value());
  EXPECT_EQ(0U, fapi_pdu.pusch_uci->beta_offset_csi2);
  EXPECT_EQ(csi.csi_part1_nof_bits, fapi_pdu.pusch_uci->csi_part1_bit.value());
}

TEST_F(mac_to_fapi_pusch_fixture, csi_without_part2_beta_offset_has_no_correspondence)
{
  pdu_test.info.uci->csi->beta_offset_csi_2.reset();

  fapi::ul_pusch_pdu fapi_pdu = convert();

  EXPECT_FALSE(fapi_pdu.uci_correspondence.has_value());
  ASSERT_TRUE(fapi_pdu.pusch_uci.has_value());
  EXPECT_EQ(0U, fapi_pdu.pusch_uci->beta_offset_csi2);
}

TEST_F(mac_to_fapi_pusch_fixture, rapid_is_absent_when_the_context_has_none)
{
  pdu_test.info.context.rapid.reset();

  fapi::ul_pusch_pdu fapi_pdu = convert();

  EXPECT_FALSE(fapi_pdu.rapid.has_value());
}

TEST_F(mac_to_fapi_pusch_fixture, tti_bundling_is_forwarded_when_the_pdu_is_part_of_a_bundle)
{
  static constexpr uint8_t nof_repetitions           = 4;
  static constexpr uint8_t nof_remaining_repetitions = 2;

  pdu_test.info.pusch_cfg.repetitions = pusch_information::repetition_info{
      .nof_repetitions = nof_repetitions, .nof_remaining_repetitions = nof_remaining_repetitions};

  fapi::ul_pusch_pdu fapi_pdu = convert();

  ASSERT_TRUE(fapi_pdu.repetitions.has_value());
  EXPECT_EQ(nof_repetitions, fapi_pdu.repetitions->tb_over_ms_num_total_slots);
  EXPECT_EQ(nof_remaining_repetitions, fapi_pdu.repetitions->tb_over_ms_remaining_slots);
}

TEST_F(mac_to_fapi_pusch_fixture, single_transmission_has_no_tti_bundling)
{
  pdu_test.info.pusch_cfg.repetitions.reset();

  fapi::ul_pusch_pdu fapi_pdu = convert();

  EXPECT_FALSE(fapi_pdu.repetitions.has_value());
}
