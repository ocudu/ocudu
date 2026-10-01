// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/du/du_cell_config_validation.h"
#include "ocudu/ran/pdcch/search_space.h"
#include <gtest/gtest.h>
#include <numeric>

using namespace ocudu;

namespace {

/// L_max of an FR2 cell, whose SSB pattern is case D.
constexpr unsigned FR2_L_MAX = 64;

class du_cell_config_ssb_validation_test : public ::testing::Test
{
protected:
  /// Returns an FR2 cell configuration, which has 64 SSB candidates.
  static odu::du_cell_config make_fr2_cell_config()
  {
    cell_config_builder_params params;
    params.scs_common = subcarrier_spacing::kHz120;
    params.dl_carrier = carrier_configuration{bs_channel_bandwidth::MHz100, 2070001, nr_band::n257, 1};

    return config_helpers::make_default_du_cell_config(params);
  }

  /// Sets the given SSB candidates as transmitted, each on its own beam.
  static void transmit_ssb_candidates(odu::du_cell_config& cell_cfg, span<const unsigned> ssb_indexes)
  {
    cell_cfg.ran.ssb_cfg.ssb_beams.reset();
    for (unsigned ssb_idx : ssb_indexes) {
      cell_cfg.ran.ssb_cfg.ssb_beams.set_beam(ssb_idx, to_beam_id(ssb_idx));
    }
  }
};

} // namespace

TEST_F(du_cell_config_ssb_validation_test, fr2_cell_has_64_ssb_candidates)
{
  ASSERT_EQ(make_fr2_cell_config().ran.ssb_cfg.ssb_beams.get_L_max(), FR2_L_MAX);
}

/// Error reported for an SSB candidate that SearchSpace#0 has no monitoring occasion for.
constexpr const char* ssb_out_of_ss0_range_error = "index of a transmitted SSB candidate";

TEST_F(du_cell_config_ssb_validation_test, ssb_candidates_within_the_searchspace0_range_are_accepted)
{
  odu::du_cell_config cell_cfg = make_fr2_cell_config();

  std::vector<unsigned> ssb_indexes(MAX_NOF_SS0_SSB_CANDIDATES);
  std::iota(ssb_indexes.begin(), ssb_indexes.end(), 0U);
  transmit_ssb_candidates(cell_cfg, ssb_indexes);

  // The rest of the FR2 cell configuration is not valid yet, so only the SSB candidates are asserted on.
  const auto result = is_du_cell_config_valid(cell_cfg);
  if (!result.has_value()) {
    ASSERT_EQ(result.error().find(ssb_out_of_ss0_range_error), std::string::npos) << result.error();
  }
}

TEST_F(du_cell_config_ssb_validation_test,
       when_an_ssb_candidate_is_outside_the_searchspace0_range_then_config_is_invalid)
{
  odu::du_cell_config cell_cfg = make_fr2_cell_config();

  // SearchSpace#0 holds no Type0-PDCCH monitoring occasion for this candidate.
  const std::array<unsigned, 2> ssb_indexes = {0U, MAX_NOF_SS0_SSB_CANDIDATES};
  transmit_ssb_candidates(cell_cfg, ssb_indexes);

  const auto result = is_du_cell_config_valid(cell_cfg);
  ASSERT_FALSE(result.has_value());
  ASSERT_NE(result.error().find(ssb_out_of_ss0_range_error), std::string::npos) << result.error();
}
