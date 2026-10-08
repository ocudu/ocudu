// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/du/du_cell_config_validation.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// Error reported for a dedicated PUCCH resource that overlaps the common PUCCH PRBs.
constexpr const char* common_ded_overlap_error = "overlaps the common PUCCH resources";

/// Row of TS 38.213 Table 9.2.1-1 that places the common PUCCH resources at N_bwp/4 from the BWP edges.
constexpr unsigned row_away_from_edges = 15;

class du_cell_config_pucch_validation_test : public ::testing::Test
{
protected:
  du_cell_config_pucch_validation_test()
  {
    cell_config_builder_params params;
    params.dl_carrier.carrier_bw = bs_channel_bandwidth::MHz20;
    cell_cfg                     = config_helpers::make_default_du_cell_config(params);
    cell_cfg.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common = row_away_from_edges;
  }

  void assert_no_overlap_error() const
  {
    // Other parts of the configuration may not fit the modified PUCCH, so only the overlap is asserted on.
    const auto result = is_du_cell_config_valid(cell_cfg);
    if (not result.has_value()) {
      ASSERT_EQ(result.error().find(common_ded_overlap_error), std::string::npos) << result.error();
    }
  }

  odu::du_cell_config cell_cfg;
};

} // namespace

TEST_F(du_cell_config_pucch_validation_test, bwp_has_106_prbs)
{
  ASSERT_EQ(cell_cfg.ran.ul_cfg_common.init_ul_bwp.generic_params.crbs.length(), 106);
}

TEST_F(du_cell_config_pucch_validation_test, default_dedicated_pucch_resources_do_not_reach_row_15_common_resources)
{
  assert_no_overlap_error();
}

TEST_F(du_cell_config_pucch_validation_test, when_dedicated_pucch_resources_reach_row_15_common_resources_then_invalid)
{
  // The dedicated PUCCH PRBs stay below 50% of the BWP, but one edge holds more than N_bwp/4 PRBs.
  auto& pucch_params                    = cell_cfg.ran.init_bwp.pucch.resources;
  pucch_params.res_set_size             = 5;
  pucch_params.nof_cell_res_set_configs = 4;
  pucch_params.f0_or_f1_params          = pucch_f1_params{.nof_syms               = 14,
                                                          .intraslot_freq_hopping = false,
                                                          .nof_cyc_shifts         = pucch_nof_cyclic_shifts::two,
                                                          .occ_supported          = true};
  pucch_f2_params f2_params;
  f2_params.nof_syms                 = 2;
  f2_params.max_nof_rbs              = 16;
  pucch_params.f2_or_f3_or_f4_params = f2_params;

  const auto result = is_du_cell_config_valid(cell_cfg);
  ASSERT_FALSE(result.has_value());
  ASSERT_NE(result.error().find(common_ded_overlap_error), std::string::npos) << result.error();
}
