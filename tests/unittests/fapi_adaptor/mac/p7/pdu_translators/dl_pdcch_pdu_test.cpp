// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "pdcch.h"
#include "ocudu/fapi_adaptor/precoding_codebook_generator.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

namespace {

/// Kind of CORESET the PDCCH PDU is transmitted in.
enum class coreset_kind { coreset0, non_coreset0_interleaved, non_coreset0_non_interleaved };

/// Fixture that converts the first DL PDCCH PDU of a valid MAC DL scheduling result.
class mac_fapi_pdcch_pdu_conversor_test : public ::testing::Test
{
  using precoding_tools =
      std::pair<std::unique_ptr<precoding_codebook_mapper>, std::unique_ptr<precoding_codebook_repository>>;

protected:
  static constexpr unsigned nof_prbs = 51;

  /// Converts the DCI context and payload of the fixture into a FAPI PDCCH PDU.
  fapi::dl_pdcch_pdu convert() const
  {
    fapi::dl_pdcch_pdu         fapi_pdu;
    fapi::dl_pdcch_pdu_builder builder(fapi_pdu);
    convert_pdcch_mac_to_fapi(builder, context, payload, *pm_tools.first, nof_prbs);
    return fapi_pdu;
  }

  const mac_dl_sched_result_test_helper result_test = build_valid_mac_dl_sched_result();
  dci_context_information               context     = result_test.result.dl_res.dl_pdcchs().front().ctx;
  const dci_payload                     payload     = result_test.result.dl_pdcch_pdus.front();
  const precoding_tools pm_tools = generate_precoding_codebooks(pmi_codebook_one_port{}, antenna_topology::one_port, 0);
};

class mac_fapi_pdcch_pdu_conversor_fixture : public mac_fapi_pdcch_pdu_conversor_test,
                                             public ::testing::WithParamInterface<coreset_kind>
{
protected:
  static coreset_configuration build_coreset(coreset_kind kind)
  {
    switch (kind) {
      case coreset_kind::coreset0:
        return build_valid_coreset0_configuration();
      case coreset_kind::non_coreset0_interleaved:
        return build_valid_non_coreset0_configuration(true);
      case coreset_kind::non_coreset0_non_interleaved:
        break;
    }
    return build_valid_non_coreset0_configuration(false);
  }
};

} // namespace

static const char* to_string(coreset_kind kind)
{
  switch (kind) {
    case coreset_kind::coreset0:
      return "coreset0";
    case coreset_kind::non_coreset0_interleaved:
      return "non_coreset0_interleaved";
    case coreset_kind::non_coreset0_non_interleaved:
      break;
  }
  return "non_coreset0_non_interleaved";
}

TEST_P(mac_fapi_pdcch_pdu_conversor_fixture, mac_to_fapi_conversion_is_valid)
{
  static constexpr unsigned non_interleaved_reg_bundle = 6;
  static constexpr int8_t   power_control_offset_ss_dB = -3;

  coreset_configuration coreset     = build_coreset(GetParam());
  context.coreset_cfg               = &coreset;
  context.tx_pwr.pwr_ctrl_offset_ss = power_control_offset_ss_dB;

  fapi::dl_pdcch_pdu fapi_pdu = convert();

  // BWP. For CORESET 0 the CORESET itself defines the bandwidth, otherwise the BWP does.
  EXPECT_EQ(context.bwp_cfg->cp, fapi_pdu.cp);
  EXPECT_EQ(context.bwp_cfg->scs, fapi_pdu.scs);
  EXPECT_EQ(GetParam() == coreset_kind::coreset0 ? coreset.coreset0_crbs() : context.bwp_cfg->crbs,
            fapi_pdu.coreset_bwp);

  // CORESET.
  EXPECT_EQ(coreset.duration(), fapi_pdu.symbols.length());
  EXPECT_EQ(context.starting_symbol, fapi_pdu.symbols.start());
  EXPECT_EQ(coreset.get_precoder_granularity(), fapi_pdu.precoder_granularity);

  // Mapping.
  switch (GetParam()) {
    case coreset_kind::coreset0: {
      const auto* mapping = std::get_if<fapi::dl_pdcch_pdu::mapping_coreset_0>(&fapi_pdu.mapping);
      ASSERT_NE(mapping, nullptr);
      ASSERT_TRUE(coreset.interleaved_mapping().has_value());
      EXPECT_EQ(*coreset.interleaved_mapping(), mapping->interleaved);
      // CORESET 0 is described by its first resources, each one a group of 6 PRBs.
      freq_resource_bitmap expected_freq_resources(pdcch_constants::MAX_NOF_FREQ_RESOURCES);
      expected_freq_resources.fill(0, coreset.coreset0_crbs().length() / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE);
      EXPECT_EQ(expected_freq_resources, fapi_pdu.freq_domain_resource);
      break;
    }
    case coreset_kind::non_coreset0_interleaved: {
      const auto* mapping = std::get_if<fapi::dl_pdcch_pdu::mapping_interleaved>(&fapi_pdu.mapping);
      ASSERT_NE(mapping, nullptr);
      ASSERT_TRUE(coreset.interleaved_mapping().has_value());
      EXPECT_EQ(*coreset.interleaved_mapping(), mapping->interleaved);
      EXPECT_EQ(coreset.freq_domain_resources(), fapi_pdu.freq_domain_resource);
      break;
    }
    case coreset_kind::non_coreset0_non_interleaved: {
      const auto* mapping = std::get_if<fapi::dl_pdcch_pdu::mapping_non_interleaved>(&fapi_pdu.mapping);
      ASSERT_NE(mapping, nullptr);
      EXPECT_EQ(non_interleaved_reg_bundle, mapping->reg_bundle_sz);
      EXPECT_EQ(coreset.freq_domain_resources(), fapi_pdu.freq_domain_resource);
      break;
    }
  }

  // DCI.
  const fapi::dl_dci_pdu& dci = fapi_pdu.dl_dci;
  EXPECT_EQ(context.rnti, dci.rnti);
  EXPECT_EQ(context.n_id_pdcch_data, dci.nid_pdcch_data);
  EXPECT_EQ(context.n_rnti_pdcch_data, dci.nrnti_pdcch_data);
  EXPECT_EQ(context.cces.ncce, dci.cce_index);
  EXPECT_EQ(context.cces.aggr_lvl, dci.dci_aggregation_level);
  EXPECT_EQ(context.n_id_pdcch_dmrs, dci.nid_pdcch_dmrs);
  EXPECT_EQ(payload, dci.payload);

  // Power.
  const auto* power = std::get_if<fapi::dl_dci_pdu::power_profile_nr>(&dci.power_config);
  ASSERT_NE(power, nullptr);
  EXPECT_EQ(power_control_offset_ss_dB, power->power_control_offset_ss_db);
}

INSTANTIATE_TEST_SUITE_P(coreset_kinds,
                         mac_fapi_pdcch_pdu_conversor_fixture,
                         ::testing::Values(coreset_kind::coreset0,
                                           coreset_kind::non_coreset0_interleaved,
                                           coreset_kind::non_coreset0_non_interleaved),
                         [](const ::testing::TestParamInfo<coreset_kind>& param_info) {
                           return to_string(param_info.param);
                         });

TEST_F(mac_fapi_pdcch_pdu_conversor_test, beamformed_dci_carries_its_beam)
{
  beam_identifier beam_id           = to_beam_id(3);
  context.precoding_and_beamforming = make_single_beam_precoding(beam_id);

  fapi::dl_pdcch_pdu fapi_pdu = convert();

  EXPECT_EQ(precoding_beam_list({beam_id}), fapi_pdu.dl_dci.precoding_and_beamforming.prg.beams);
}

TEST_F(mac_fapi_pdcch_pdu_conversor_test, dci_without_a_beam_selects_a_precoding_matrix)
{
  context.precoding_and_beamforming = make_default_precoding();

  fapi::dl_pdcch_pdu fapi_pdu = convert();

  const auto& precoding = fapi_pdu.dl_dci.precoding_and_beamforming.prg;
  EXPECT_TRUE(precoding.beams.empty());
  const auto* pmi = std::get_if<fapi::precoding_matrix_index>(&precoding.precoding);
  ASSERT_NE(pmi, nullptr);
  EXPECT_EQ(pm_tools.first->map(mac_pdcch_precoding_info{}), *pmi);
}

TEST_F(mac_fapi_pdcch_pdu_conversor_test, dci_context_carries_the_search_space_format_and_harq_feedback_timing)
{
  static constexpr unsigned harq_feedback_timing_slots = 4;
  search_space_id           ss_id                      = to_search_space_id(3);
  const char*               dci_format                 = "1_0";

  context.context.ss_id                = ss_id;
  context.context.dci_format           = dci_format;
  context.context.harq_feedback_timing = harq_feedback_timing_slots;

  fapi::dl_pdcch_pdu fapi_pdu = convert();

  EXPECT_EQ(pdcch_context(ss_id, dci_format, harq_feedback_timing_slots), fapi_pdu.dl_dci.context);
}

TEST_F(mac_fapi_pdcch_pdu_conversor_test, dci_context_without_harq_feedback_timing_has_none)
{
  search_space_id ss_id      = to_search_space_id(1);
  const char*     dci_format = "0_0";

  context.context.ss_id      = ss_id;
  context.context.dci_format = dci_format;
  context.context.harq_feedback_timing.reset();

  fapi::dl_pdcch_pdu fapi_pdu = convert();

  EXPECT_EQ(pdcch_context(ss_id, dci_format, std::nullopt), fapi_pdu.dl_dci.context);
}
