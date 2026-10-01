// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "ocudu/ran/prs/prs.h"
#include "ocudu/scheduler/config/periodic_resource_sched_validator.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// Builds a minimal, otherwise valid, PRS resource set with a single resource.
prs_resource_set make_prs_resource_set(uint16_t start_prb,
                                       uint16_t bandwidth_prbs,
                                       unsigned periodicity_slots,
                                       unsigned slot_offset,
                                       uint8_t  re_offset,
                                       uint8_t  symbol_offset)
{
  prs_resource_set res_set{};
  res_set.start_prb         = start_prb;
  res_set.bandwidth_prbs    = bandwidth_prbs;
  res_set.comb_size         = prs_comb_size::two;
  res_set.periodicity_slots = periodicity_slots;
  res_set.slot_offset       = slot_offset;
  res_set.repetition_factor = prs_repetition_factor::one;
  res_set.time_gap          = prs_time_gap::one;
  res_set.nof_symbols       = prs_num_symbols::two;
  res_set.power_offset_db   = 0;
  res_set.resources.push_back(
      prs_resource{.sequence_id = 0, .re_offset = re_offset, .slot_offset = 0, .symbol_offset = symbol_offset});
  return res_set;
}

} // namespace

TEST(periodic_resource_sched_validator_test, default_cell_config_has_no_periodic_resource_collisions)
{
  const ran_cell_config ran = sched_config_helper::make_default_sched_cell_configuration_request().ran;
  ASSERT_TRUE(check_periodic_resource_collisions(ran).has_value());
}

TEST(periodic_resource_sched_validator_test, csi_resource_freq_band_wider_than_the_cell_bandwidth_is_capped_at_the_bwp)
{
  // csi-FrequencyOccupation only allows a number of RBs that is a multiple of 4, so in a 273-CRB cell the CSI-RS and
  // CSI-IM frequency bands span 276 CRBs, i.e. past the end of the DL BWP.
  const cell_config_builder_params params =
      cell_config_builder_profiles::create(duplex_mode::TDD, frequency_range::FR1, bs_channel_bandwidth::MHz100);
  const ran_cell_config ran = sched_config_helper::make_default_sched_cell_configuration_request(params).ran;
  ASSERT_EQ(ran.dl_cfg_common.init_dl_bwp.generic_params.crbs.length(), 273);
  ASSERT_TRUE(check_periodic_resource_collisions(ran).has_value());
}

TEST(periodic_resource_sched_validator_test, prs_resources_on_same_crbs_symbols_and_comb_offset_collide)
{
  ran_cell_config ran = sched_config_helper::make_default_sched_cell_configuration_request().ran;
  ran.prs_cfg.resource_sets.push_back(make_prs_resource_set(0, 24, 10, 0, 0, 0));
  ran.prs_cfg.resource_sets.push_back(make_prs_resource_set(0, 24, 10, 0, 0, 0));
  ASSERT_FALSE(check_periodic_resource_collisions(ran).has_value());
}

TEST(periodic_resource_sched_validator_test, prs_resources_on_disjoint_comb_offsets_do_not_collide)
{
  // Same CRBs, symbols and periodicity, but interleaved on the frequency-domain comb (re_offset 0 vs 1 of a comb-2
  // pattern), as is standard practice to multiplex several DL-PRS resources without colliding.
  ran_cell_config ran = sched_config_helper::make_default_sched_cell_configuration_request().ran;
  ran.prs_cfg.resource_sets.push_back(make_prs_resource_set(0, 24, 10, 0, 0, 0));
  ran.prs_cfg.resource_sets.push_back(make_prs_resource_set(0, 24, 10, 0, 1, 0));
  ASSERT_TRUE(check_periodic_resource_collisions(ran).has_value());
}

TEST(periodic_resource_sched_validator_test, prs_resources_on_disjoint_crbs_do_not_collide)
{
  ran_cell_config ran = sched_config_helper::make_default_sched_cell_configuration_request().ran;
  ran.prs_cfg.resource_sets.push_back(make_prs_resource_set(0, 24, 10, 0, 0, 0));
  ran.prs_cfg.resource_sets.push_back(make_prs_resource_set(28, 24, 10, 0, 0, 0));
  ASSERT_TRUE(check_periodic_resource_collisions(ran).has_value());
}

TEST(periodic_resource_sched_validator_test, prs_resources_with_different_periods_collide_when_slot_aligned)
{
  // gcd(8, 20) = 4; offsets 0 and 4 share the same residue mod 4, so the two resources eventually land on the same
  // slot, and their CRBs, symbols and comb offset all match.
  ran_cell_config ran = sched_config_helper::make_default_sched_cell_configuration_request().ran;
  ran.prs_cfg.resource_sets.push_back(make_prs_resource_set(0, 24, 8, 0, 0, 0));
  ran.prs_cfg.resource_sets.push_back(make_prs_resource_set(0, 24, 20, 4, 0, 0));
  ASSERT_FALSE(check_periodic_resource_collisions(ran).has_value());
}

TEST(periodic_resource_sched_validator_test, prs_resources_with_different_periods_do_not_collide_when_slot_misaligned)
{
  // gcd(8, 20) = 4; offsets 0 and 2 never share the same residue mod 4, so the two resources can never land on the
  // same slot, regardless of their CRBs, symbols or comb offset.
  ran_cell_config ran = sched_config_helper::make_default_sched_cell_configuration_request().ran;
  ran.prs_cfg.resource_sets.push_back(make_prs_resource_set(0, 24, 8, 0, 0, 0));
  ran.prs_cfg.resource_sets.push_back(make_prs_resource_set(0, 24, 20, 2, 0, 0));
  ASSERT_TRUE(check_periodic_resource_collisions(ran).has_value());
}

namespace {

/// Builds TDD cell build parameters where slot 2 (out of a 10-slot period) is a special slot with only 6 DL symbols
/// available (symbols [6, 14) are not available for DL), and CSI-RS is disabled to keep the DL resource grid free of
/// anything but SSB and the PRS resources added by the tests.
cell_config_builder_params make_tdd_cell_cfg_params()
{
  cell_config_builder_params params = cell_config_builder_profiles::create(duplex_mode::TDD);
  params.csi_rs_enabled             = false;

  tdd_ul_dl_config_common& tdd_cfg           = params.tdd_ul_dl_cfg_common.emplace();
  tdd_cfg.ref_scs                            = params.scs_common;
  tdd_cfg.pattern1.dl_ul_tx_period_nof_slots = 10;
  tdd_cfg.pattern1.nof_dl_slots              = 2;
  tdd_cfg.pattern1.nof_dl_symbols            = 6;
  tdd_cfg.pattern1.nof_ul_slots              = 7;
  tdd_cfg.pattern1.nof_ul_symbols            = 0;

  return params;
}

} // namespace

TEST(periodic_resource_sched_validator_test, prs_resource_outside_tdd_active_dl_symbols_collides_with_tdd_pattern)
{
  const cell_config_builder_params params = make_tdd_cell_cfg_params();
  ran_cell_config                  ran = sched_config_helper::make_default_sched_cell_configuration_request(params).ran;

  // Symbols [6, 8) of the special slot (slot 2) are not available for DL.
  ran.prs_cfg.resource_sets.push_back(make_prs_resource_set(0, 24, 10, 2, 0, 6));
  ASSERT_FALSE(check_periodic_resource_collisions(ran).has_value());
}

TEST(periodic_resource_sched_validator_test,
     prs_resource_within_tdd_active_dl_symbols_does_not_collide_with_tdd_pattern)
{
  const cell_config_builder_params params = make_tdd_cell_cfg_params();
  ran_cell_config                  ran = sched_config_helper::make_default_sched_cell_configuration_request(params).ran;

  // Symbols [0, 2) of the special slot (slot 2) are available for DL.
  ran.prs_cfg.resource_sets.push_back(make_prs_resource_set(0, 24, 10, 2, 0, 0));
  ASSERT_TRUE(check_periodic_resource_collisions(ran).has_value());
}

TEST(periodic_resource_sched_validator_test, short_preamble_prach_burst_does_not_collide_with_itself)
{
  const cell_config_builder_params params = make_tdd_cell_cfg_params();
  ran_cell_config                  ran = sched_config_helper::make_default_sched_cell_configuration_request(params).ran;

  // PRACH configuration index 77 uses a short preamble whose burst spans 2 slots at SCS 30kHz. Both slots are already
  // flagged as PRACH occasions, and they fall in UL slots of the TDD pattern, so nothing collides.
  ran.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index = 77;
  ASSERT_TRUE(check_periodic_resource_collisions(ran).has_value());
}
