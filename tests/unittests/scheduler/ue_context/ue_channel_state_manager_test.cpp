// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/ue_context/ue_channel_state_manager.h"
#include "ocudu/ran/pdsch/pdsch_constants.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

ue_channel_state_manager make_channel_state_manager(unsigned nof_dl_ports)
{
  return ue_channel_state_manager(config_helpers::make_default_scheduler_expert_config().ue, nof_dl_ports);
}

} // namespace

// With a single antenna port no precoding is applied.
TEST(ue_channel_state_manager_test, single_port_uses_no_precoding)
{
  const ue_channel_state_manager csm = make_channel_state_manager(1);

  EXPECT_EQ(csm.get_nof_dl_layers(), 1);
  const precoding_and_beamforming_info precoding = csm.get_precoding(1);
  ASSERT_TRUE(std::holds_alternative<precoding_matrix_indicator>(precoding));
  EXPECT_TRUE(std::holds_alternative<std::monostate>(std::get<precoding_matrix_indicator>(precoding)));
}

// With 2 antenna ports the initial precoding uses a two-antenna-port PMI for every supported number of layers.
TEST(ue_channel_state_manager_test, two_ports_use_two_antenna_port_pmi)
{
  const ue_channel_state_manager csm = make_channel_state_manager(2);

  EXPECT_EQ(csm.get_nof_dl_layers(), 1);
  for (unsigned nof_layers = 1; nof_layers <= 2; ++nof_layers) {
    const precoding_and_beamforming_info precoding = csm.get_precoding(nof_layers);
    ASSERT_TRUE(std::holds_alternative<precoding_matrix_indicator>(precoding));
    EXPECT_TRUE(std::holds_alternative<pmi_two_antenna_port>(std::get<precoding_matrix_indicator>(precoding)))
        << "unexpected PMI type for nof_layers=" << nof_layers;
  }
}

// With 4 antenna ports the initial precoding uses the Type-I single-panel two_one (N1=2, N2=1) codebook.
TEST(ue_channel_state_manager_test, four_ports_use_two_one_codebook)
{
  const ue_channel_state_manager csm = make_channel_state_manager(4);

  EXPECT_EQ(csm.get_nof_dl_layers(), 1);
  for (unsigned nof_layers = 1; nof_layers <= 4; ++nof_layers) {
    const precoding_and_beamforming_info precoding = csm.get_precoding(nof_layers);
    ASSERT_TRUE(std::holds_alternative<precoding_matrix_indicator>(precoding));
    EXPECT_EQ(std::get<pmi_typeI_single_panel>(std::get<precoding_matrix_indicator>(precoding)).panel_config.n1_n2,
              pmi_codebook_single_panel_config::two_one)
        << "unexpected codebook for nof_layers=" << nof_layers;
  }
}

// With 8 antenna ports the initial precoding uses the Type-I single-panel four_one (N1=4, N2=1) codebook for every
// supported number of layers, which is capped at a single codeword limit (MAX_NOF_LAYERS_PER_CODEWORD).
TEST(ue_channel_state_manager_test, eight_ports_use_four_one_codebook)
{
  const ue_channel_state_manager csm = make_channel_state_manager(8);

  EXPECT_EQ(csm.get_nof_dl_layers(), 1);
  for (unsigned nof_layers = 1; nof_layers <= pdsch_constants::MAX_NOF_LAYERS_PER_CODEWORD; ++nof_layers) {
    const precoding_and_beamforming_info precoding = csm.get_precoding(nof_layers);
    ASSERT_TRUE(std::holds_alternative<precoding_matrix_indicator>(precoding));
    EXPECT_EQ(std::get<pmi_typeI_single_panel>(std::get<precoding_matrix_indicator>(precoding)).panel_config.n1_n2,
              pmi_codebook_single_panel_config::four_one)
        << "unexpected codebook for nof_layers=" << nof_layers;
  }
}

// An RI above the single-codeword layer limit is clamped to MAX_NOF_LAYERS_PER_CODEWORD.
TEST(ue_channel_state_manager_test, ri_above_single_codeword_limit_is_limited)
{
  ue_channel_state_manager csm = make_channel_state_manager(8);

  csi_report_data report;
  report.ri = csi_report_data::ri_type{6};
  report.pmi =
      pmi_typeI_single_panel{.panel_config = {pmi_codebook_single_panel_config::four_one, pmi_codebook_typeI_mode::one},
                             .i_1_1        = 0,
                             .i_1_2        = std::nullopt,
                             .i_1_3        = std::nullopt,
                             .i_2          = 0};

  EXPECT_TRUE(csm.handle_csi_report(report));
  EXPECT_EQ(csm.get_nof_dl_layers(), pdsch_constants::MAX_NOF_LAYERS_PER_CODEWORD);
}

// An RI within the single-codeword layer limit is used verbatim.
TEST(ue_channel_state_manager_test, ri_within_limit_is_used)
{
  ue_channel_state_manager csm = make_channel_state_manager(8);

  csi_report_data report;
  report.ri = csi_report_data::ri_type{3};

  EXPECT_TRUE(csm.handle_csi_report(report));
  EXPECT_EQ(csm.get_nof_dl_layers(), 3);
}

// An RI that exceeds the number of DL ports is invalid and the report is rejected.
TEST(ue_channel_state_manager_test, ri_above_nof_ports_is_rejected)
{
  ue_channel_state_manager csm = make_channel_state_manager(4);

  csi_report_data report;
  report.ri = csi_report_data::ri_type{5};

  EXPECT_FALSE(csm.handle_csi_report(report));
}

// The recommended beam is recorded for the fallback scheduler and does not change the reported precoding.
TEST(ue_channel_state_manager_test, the_recommended_beam_does_not_change_the_precoding)
{
  // A beam other than the first one, so that the assertions discriminate against a hardcoded default.
  constexpr beam_identifier recommended_beam = static_cast<beam_identifier>(3);

  ue_channel_state_manager csm = make_channel_state_manager(4);
  csm.set_recommended_beam(recommended_beam);

  EXPECT_EQ(csm.get_recommended_beam(), recommended_beam);

  for (unsigned nof_layers = 1; nof_layers <= 4; ++nof_layers) {
    EXPECT_TRUE(std::holds_alternative<precoding_matrix_indicator>(csm.get_precoding(nof_layers)))
        << "unexpected beamforming for nof_layers=" << nof_layers;
  }
}

// A beam is not recommended before the UE reaches the cell on an SS/PBCH block.
TEST(ue_channel_state_manager_test, no_beam_is_recommended_by_default)
{
  EXPECT_FALSE(make_channel_state_manager(4).get_recommended_beam().has_value());
}
